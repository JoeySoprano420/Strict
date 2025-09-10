"""
Command-line interface for the Strict package.
"""

import argparse
import sys
from pathlib import Path
from typing import Optional

from .downloader import StrictDownloader, StrictError
from .core import StrictValidator


def download_command(args) -> int:
    """Handle the download command."""
    try:
        with StrictDownloader(timeout=args.timeout, max_size=args.max_size) as downloader:
            if args.info:
                # Just get file info
                info = downloader.get_file_info(args.url)
                print(f"URL: {info['url']}")
                print(f"Content-Type: {info['content_type']}")
                print(f"Content-Length: {info['content_length']} bytes")
                print(f"Last-Modified: {info['last_modified']}")
                return 0
            else:
                # Download the file
                destination = Path(args.output) if args.output else None
                result_path = downloader.download_file(
                    args.url, 
                    destination, 
                    overwrite=args.overwrite
                )
                print(f"Downloaded successfully: {result_path}")
                return 0
                
    except StrictError as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1
    except Exception as e:
        print(f"Unexpected error: {e}", file=sys.stderr)
        return 1


def validate_command(args) -> int:
    """Handle the validate command."""
    try:
        validator = StrictValidator()
        
        # Add some default rules
        if args.type == 'int':
            result = validator.require_type(int(args.value), int)
        elif args.type == 'str':
            result = validator.require_type(args.value, str)
        elif args.type == 'nonempty':
            result = validator.require_non_empty(args.value)
        else:
            print(f"Unknown validation type: {args.type}", file=sys.stderr)
            return 1
        
        print(f"Validation passed: {result}")
        return 0
        
    except StrictError as e:
        print(f"Validation failed: {e}", file=sys.stderr)
        return 1
    except Exception as e:
        print(f"Unexpected error: {e}", file=sys.stderr)
        return 1


def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        prog='strict',
        description='Strict validation and download utility'
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Download command
    download_parser = subparsers.add_parser('download', help='Download files with strict validation')
    download_parser.add_argument('url', help='URL to download')
    download_parser.add_argument('-o', '--output', help='Output file path')
    download_parser.add_argument('--overwrite', action='store_true', 
                               help='Overwrite existing files')
    download_parser.add_argument('--timeout', type=int, default=30, 
                               help='Download timeout in seconds')
    download_parser.add_argument('--max-size', type=int, default=100*1024*1024,
                               help='Maximum file size in bytes')
    download_parser.add_argument('--info', action='store_true',
                               help='Only get file information, do not download')
    
    # Validate command
    validate_parser = subparsers.add_parser('validate', help='Validate values with strict rules')
    validate_parser.add_argument('type', choices=['int', 'str', 'nonempty'], 
                               help='Validation type')
    validate_parser.add_argument('value', help='Value to validate')
    
    # Version command
    version_parser = subparsers.add_parser('version', help='Show version information')
    
    args = parser.parse_args()
    
    if args.command == 'download':
        return download_command(args)
    elif args.command == 'validate':
        return validate_command(args)
    elif args.command == 'version':
        from . import __version__
        print(f"strict {__version__}")
        return 0
    else:
        parser.print_help()
        return 1


if __name__ == '__main__':
    sys.exit(main())