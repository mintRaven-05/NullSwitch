#!/bin/bash

# NullSwitch v1.0.0 Installation Script
# This script installs NullSwitch and its dependencies

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Installation paths
INSTALL_DIR="$HOME/.local/share/nullswitch"
BIN_PATH="/usr/bin/nullswitch"

print_info() {
  echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
  echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
  echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
  echo -e "${RED}[ERROR]${NC} $1"
}

# Function to detect package manager
detect_package_manager() {
  if command -v apt &>/dev/null; then
    echo "apt"
  elif command -v yum &>/dev/null; then
    echo "yum"
  elif command -v dnf &>/dev/null; then
    echo "dnf"
  elif command -v pacman &>/dev/null; then
    echo "pacman"
  elif command -v zypper &>/dev/null; then
    echo "zypper"
  elif command -v apk &>/dev/null; then
    echo "apk"
  else
    echo "unknown"
  fi
}

# Function to install picocom
install_picocom() {
  local pkg_manager="$1"

  print_info "Installing picocom using $pkg_manager..."

  case "$pkg_manager" in
  "apt")
    sudo apt update && sudo apt install -y picocom
    ;;
  "yum")
    sudo yum install -y picocom
    ;;
  "dnf")
    sudo dnf install -y picocom
    ;;
  "pacman")
    sudo pacman -S --noconfirm picocom
    ;;
  "zypper")
    sudo zypper install -y picocom
    ;;
  "apk")
    sudo apk add picocom
    ;;
  *)
    print_error "Unknown package manager. Please install picocom manually."
    exit 1
    ;;
  esac
}

# Function to create the nullswitch script
create_nullswitch_script() {
  cat >"$SCRIPT_DIR/nullswitch" <<'EOF'
#!/bin/bash

# NullSwitch v1.0.0 - ESP8266 Serial Communication Tool

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

INSTALL_DIR="$HOME/.local/share/nullswitch"

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

show_help() {
    echo "NullSwitch v1.0.0 - ESP8266 Serial Communication Tool"
    echo ""
    echo "Usage: nullswitch [OPTIONS] [PORT]"
    echo ""
    echo "Options:"
    echo "  -h, --help      Show this help message"
    echo "  -v, --version   Show version information"
    echo "  --uninstall     Uninstall NullSwitch"
    echo ""
    echo "Examples:"
    echo "  nullswitch                    # Interactive mode - prompts for port"
    echo "  nullswitch /dev/ttyUSB0       # Connect directly to specified port"
    echo "  nullswitch --uninstall        # Uninstall NullSwitch"
}

show_version() {
    if [[ -f "$INSTALL_DIR/VERSION" ]]; then
        cat "$INSTALL_DIR/VERSION"
    else
        echo "Error: VERSION file not found at $INSTALL_DIR/VERSION"
        exit 1
    fi
}

uninstall_nullswitch() {
    if [[ -f "$INSTALL_DIR/uninstall.sh" ]]; then
        print_info "Running uninstall script..."
        bash "$INSTALL_DIR/uninstall.sh"
    else
        print_error "Uninstall script not found at $INSTALL_DIR/uninstall.sh"
        exit 1
    fi
}

# Check if picocom is installed
if ! command -v picocom &> /dev/null; then
    print_error "picocom is not installed. Please install it first."
    exit 1
fi

# Parse command line arguments
case "${1:-}" in
    -h|--help)
        show_help
        exit 0
        ;;
    -v|--version)
        show_version
        exit 0
        ;;
    --uninstall)
        uninstall_nullswitch
        exit 0
        ;;
    "")
        # Interactive mode - ask for port
        echo "NullSwitch v1.0.0 - ESP8266 Serial Communication Tool"
        echo ""
        echo "Available serial ports:"
        ls /dev/tty* 2>/dev/null | grep -E "(USB|ACM)" || echo "No USB/ACM ports found"
        echo ""
        read -p "Enter ESP8266 port (e.g., /dev/ttyUSB0): " PORT
        ;;
    *)
        # Port provided as argument
        PORT="$1"
        ;;
esac

# Validate port
if [[ -z "$PORT" ]]; then
    print_error "No port specified"
    exit 1
fi

if [[ ! -e "$PORT" ]]; then
    print_error "Port $PORT does not exist"
    exit 1
fi

# Connect to ESP8266
print_info "Connecting to ESP8266 on $PORT at 115200 baud..."
print_info "Press Ctrl+A then Ctrl+X to exit picocom"
echo ""

# Run picocom
picocom -b 115200 "$PORT"
EOF

  chmod +x "$SCRIPT_DIR/nullswitch"
}

# Function to check project files
check_project_files() {
  local missing_files=()

  if [[ ! -f "$SCRIPT_DIR/VERSION" ]]; then
    missing_files+=("VERSION")
  fi

  if [[ ! -f "$SCRIPT_DIR/README.md" ]]; then
    missing_files+=("README.md")
  fi

  if [[ ! -f "$SCRIPT_DIR/LICENSE" ]]; then
    missing_files+=("LICENSE")
  fi

  if [[ ${#missing_files[@]} -gt 0 ]]; then
    print_error "Missing required project files: ${missing_files[*]}"
    print_info "Please ensure VERSION, README.md, and LICENSE files are in the project directory."
    exit 1
  fi

  print_success "All required project files found"
}

# Function to create uninstall script
create_uninstall_script() {
  cat >"$SCRIPT_DIR/uninstall.sh" <<'EOF'
#!/bin/bash

# NullSwitch v1.0.0 Uninstall Script

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

echo "NullSwitch v1.0.0 Uninstaller"
echo "=============================="
echo ""

# Confirm uninstallation
read -p "Are you sure you want to uninstall NullSwitch? (y/N): " -n 1 -r
echo ""

if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    print_info "Uninstallation cancelled."
    exit 0
fi

print_info "Uninstalling NullSwitch..."

# Remove the binary from /usr/bin
if [[ -f "/usr/bin/nullswitch" ]]; then
    print_info "Removing /usr/bin/nullswitch..."
    sudo rm -f "/usr/bin/nullswitch"
    print_success "Removed /usr/bin/nullswitch"
else
    print_warning "/usr/bin/nullswitch not found"
fi

# Remove the installation directory
INSTALL_DIR="$HOME/.local/share/nullswitch"
if [[ -d "$INSTALL_DIR" ]]; then
    print_info "Removing $INSTALL_DIR..."
    rm -rf "$INSTALL_DIR"
    print_success "Removed $INSTALL_DIR"
else
    print_warning "$INSTALL_DIR not found"
fi

print_success "NullSwitch has been successfully uninstalled!"
print_info "Note: picocom was not removed as it may be used by other applications."
EOF

  chmod +x "$SCRIPT_DIR/uninstall.sh"
}

# Main installation function
main() {
  echo "NullSwitch v1.0.0 Installer"
  echo "============================"
  echo ""

  # Check if running as root
  if [[ $EUID -eq 0 ]]; then
    print_error "Please do not run this script as root. Run as a regular user."
    print_info "sudo will be prompted when needed."
    exit 1
  fi

  # Check if picocom is already installed
  if command -v picocom &>/dev/null; then
    print_success "picocom is already installed"
  else
    print_info "picocom not found, installing..."
    pkg_manager=$(detect_package_manager)

    if [[ "$pkg_manager" == "unknown" ]]; then
      print_error "Could not detect package manager. Please install picocom manually."
      exit 1
    fi

    print_info "Detected package manager: $pkg_manager"
    install_picocom "$pkg_manager"
    print_success "picocom installed successfully"
  fi

  # Check for required project files
  print_info "Checking project files..."
  check_project_files

  # Create generated files
  print_info "Creating nullswitch script and uninstall script..."
  create_nullswitch_script
  create_uninstall_script
  print_success "Generated files created"

  # Create installation directory
  print_info "Creating installation directory..."
  mkdir -p "$INSTALL_DIR"

  # Copy files to installation directory
  print_info "Installing files..."
  cp "$SCRIPT_DIR/uninstall.sh" "$INSTALL_DIR/"
  cp "$SCRIPT_DIR/VERSION" "$INSTALL_DIR/"
  cp "$SCRIPT_DIR/README.md" "$INSTALL_DIR/"
  cp "$SCRIPT_DIR/LICENSE" "$INSTALL_DIR/"

  # Install the main script to /usr/bin
  print_info "Installing nullswitch to /usr/bin..."
  sudo cp "$SCRIPT_DIR/nullswitch" "$BIN_PATH"
  sudo chmod +x "$BIN_PATH"

  print_success "NullSwitch v1.0.0 has been installed successfully!"
  echo ""
  print_info "Usage:"
  echo "  nullswitch                # Interactive mode"
  echo "  nullswitch /dev/ttyUSB0   # Direct connection"
  echo "  nullswitch --help         # Show help"
  echo "  nullswitch --uninstall    # Uninstall"
  echo ""
  print_info "Installation location: $INSTALL_DIR"
  print_info "Binary location: $BIN_PATH"
}

# Run main function
main "$@"
