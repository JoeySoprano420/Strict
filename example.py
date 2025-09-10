#!/usr/bin/env python3
"""
Example usage of the Strict package.
"""

from strict import StrictValidator, StrictDownloader, StrictError
from pathlib import Path


def main():
    print("=== Strict Package Example ===\n")
    
    # 1. Validation Examples
    print("1. Validation Examples:")
    validator = StrictValidator()
    
    try:
        # Type validation
        result = validator.require_type(42, int)
        print(f"  ✓ Type validation (int): {result}")
        
        result = validator.require_type("hello", str)
        print(f"  ✓ Type validation (str): {result}")
        
        # Non-empty validation
        result = validator.require_non_empty("not empty")
        print(f"  ✓ Non-empty validation: '{result}'")
        
        # Custom rules
        validator.add_rule("positive", lambda x: x > 0)
        validator.add_rule("even", lambda x: x % 2 == 0)
        
        result = validator.strict_validate("positive", 42)
        print(f"  ✓ Custom rule (positive): {result}")
        
        result = validator.strict_validate("even", 42)
        print(f"  ✓ Custom rule (even): {result}")
        
    except StrictError as e:
        print(f"  ✗ Validation error: {e}")
    
    print()
    
    # 2. Download Examples (without actual network requests)
    print("2. Download Examples:")
    
    try:
        downloader = StrictDownloader()
        
        # URL validation
        url = downloader.validate_url("https://example.com/file.txt")
        print(f"  ✓ URL validation: {url}")
        
        # File path validation
        temp_path = Path("/tmp/strict_example.txt")
        validated_path = downloader.validate_file_path(temp_path)
        print(f"  ✓ File path validation: {validated_path}")
        
        # Note: Actual downloads would require network access
        print("  ℹ  Network downloads require internet access")
        
    except StrictError as e:
        print(f"  ✗ Download error: {e}")
    
    print()
    
    # 3. Error Handling Examples
    print("3. Error Handling Examples:")
    
    try:
        # This will fail
        validator.require_type("42", int)
    except StrictError as e:
        print(f"  ✓ Caught expected error: {e}")
    
    try:
        # This will fail
        validator.require_non_empty("")
    except StrictError as e:
        print(f"  ✓ Caught expected error: {e}")
    
    try:
        # This will fail
        downloader.validate_url("invalid-url")
    except StrictError as e:
        print(f"  ✓ Caught expected error: {e}")
    
    print("\n=== Example Complete ===")


if __name__ == "__main__":
    main()