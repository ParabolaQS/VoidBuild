#!/usr/bin/env bash
#
# install.sh - Automated setup for custom Void Linux terminal tools
#

set -euo pipefail

TARGET_FILE="$HOME/.bashrc"
BACKUP_FILE="$HOME/.bashrc.bak.$(date +%F_%T)"
MARKER="# --- VOID POWER USER UTILITIES ---"

echo "==> Preparing to inject utilities into $TARGET_FILE..."

# Ensure target file exists
if [ ! -f "$TARGET_FILE" ]; then
    echo "[-] $TARGET_FILE not found. Creating a fresh one."
    touch "$TARGET_FILE"
fi

# Prevent duplicate injections
if grep -qF "$MARKER" "$TARGET_FILE"; then
    echo "[!] Utilities already present in $TARGET_FILE. Aborting to avoid duplicates."
    exit 0
fi

# Create a safe rollback backup
echo "==> Creating system backup at $BACKUP_FILE"
cp "$TARGET_FILE" "$BACKUP_FILE"

# Append the block cleanly
echo "==> Writing logic to configuration file..."
cat << 'EOF' >> "$TARGET_FILE"

# --- VOID POWER USER UTILITIES ---
# Safe ANSI colors that won't break terminal layouts
B_BLUE='\033[1;34m'
B_GREEN='\033[1;32m'
B_YELLOW='\033[1;33m'
B_RED='\033[1;31m'
NC='\033[0m'

# Ultimate System Maintenance Pipeline
vup() {
    echo -e "${B_BLUE}==> Syncing repositories and upgrading system...${NC}"
    if sudo xbps-install -Syu; then
        echo -e "${B_BLUE}==> Cleaning package cache...${NC}"
        sudo xbps-remove -O
        
        local orphans
        orphans=$(xbps-query -O)
        if [ -n "$orphans" ]; then
            echo -e "${B_YELLOW}==> Found orphan dependencies:${NC}"
            echo "$orphans"
            echo -n "Would you like to purge them? (y/N): "
            read -r res
            if [[ "$res" =~ ^([yY][eE][sS]|[yY])$ ]]; then
                sudo xbps-remove -oo
            fi
        else
            echo -e "${B_GREEN}[+] System is clean. No orphans found.${NC}"
        fi
    fi
}

# System Audit Tool - Lists explicit user-installed packages
vlist() {
    echo -e "${B_BLUE}=== Explicitly Installed Packages ===${NC}"
    xbps-query -m
}

# Visual Color-Coded Runit Service Monitor
vstat() {
    echo -e "${B_BLUE}=== Active Runit Services ===${NC}"
    for srv in /var/service/*; do
        if [ -L "$srv" ]; then
            local name
            name=$(basename "$srv")
            if sudo sv status "$name" | grep -q "^run:"; then
                echo -e "  ${B_GREEN}[RUNNING]${NC}  $name"
            else
                echo -e "  ${B_RED}[DOWN]${NC}     $name"
            fi
        fi
    done
}

# Quick Environment Navigation
conf() {
    case "$1" in
        bash)   ${EDITOR:-nano} ~/.bashrc ;;
        conf)   cd ~/.config || return; ls -A ;;
        *)      echo "Usage: conf [bash|conf]" ;;
    esac
}

# Practical Quality-of-Life Tools
alias ...='cd ../..'
alias ....='cd ../../..'
alias grep='grep --color=auto'
alias mkdir='mkdir -pv'
# --- END VOID POWER USER UTILITIES ---
EOF

echo -e "\n[+] Installation complete! Rollback backup stored at: $BACKUP_FILE"
echo "[+] Reload your environment with: source $TARGET_FILE"
