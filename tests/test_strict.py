"""
Tests for the strict package.
"""

import pytest
from pathlib import Path
import tempfile
import os

from strict import StrictValidator, StrictError, StrictDownloader


class TestStrictValidator:
    """Tests for StrictValidator class."""
    
    def test_require_type_success(self):
        """Test successful type validation."""
        validator = StrictValidator()
        
        result = validator.require_type(42, int)
        assert result == 42
        
        result = validator.require_type("hello", str)
        assert result == "hello"
    
    def test_require_type_failure(self):
        """Test type validation failure."""
        validator = StrictValidator()
        
        with pytest.raises(StrictError, match="Expected type int, got str"):
            validator.require_type("42", int)
    
    def test_require_not_none(self):
        """Test None validation."""
        validator = StrictValidator()
        
        result = validator.require_not_none("hello")
        assert result == "hello"
        
        with pytest.raises(StrictError, match="Value cannot be None"):
            validator.require_not_none(None)
    
    def test_require_non_empty(self):
        """Test non-empty validation."""
        validator = StrictValidator()
        
        result = validator.require_non_empty("hello")
        assert result == "hello"
        
        result = validator.require_non_empty([1, 2, 3])
        assert result == [1, 2, 3]
        
        with pytest.raises(StrictError, match="Value cannot be empty"):
            validator.require_non_empty("")
        
        with pytest.raises(StrictError, match="Value cannot be empty"):
            validator.require_non_empty([])
    
    def test_custom_rules(self):
        """Test custom validation rules."""
        validator = StrictValidator()
        
        # Add a rule for positive numbers
        validator.add_rule("positive", lambda x: x > 0)
        
        assert validator.validate("positive", 42) is True
        assert validator.validate("positive", -1) is False
        
        # Test strict validation
        result = validator.strict_validate("positive", 42)
        assert result == 42
        
        with pytest.raises(StrictError, match="Validation failed"):
            validator.strict_validate("positive", -1)


class TestStrictDownloader:
    """Tests for StrictDownloader class."""
    
    def test_validate_url(self):
        """Test URL validation."""
        downloader = StrictDownloader()
        
        # Valid URLs
        result = downloader.validate_url("https://example.com/file.txt")
        assert result == "https://example.com/file.txt"
        
        result = downloader.validate_url("http://example.com/file.txt")
        assert result == "http://example.com/file.txt"
        
        # Invalid URLs
        with pytest.raises(StrictError, match="Invalid URL format"):
            downloader.validate_url("not-a-url")
        
        with pytest.raises(StrictError, match="Unsupported URL scheme"):
            downloader.validate_url("ftp://example.com/file.txt")
    
    def test_validate_file_path(self):
        """Test file path validation."""
        downloader = StrictDownloader()
        
        with tempfile.TemporaryDirectory() as temp_dir:
            file_path = Path(temp_dir) / "test" / "file.txt"
            
            result = downloader.validate_file_path(file_path)
            assert result == file_path
            assert result.parent.exists()  # Parent directory should be created
    
    def test_download_to_temp(self):
        """Test downloading to temporary location."""
        # This is a mock test since we can't rely on external URLs in tests
        downloader = StrictDownloader()
        
        # Test URL validation part
        with pytest.raises(StrictError, match="Invalid URL format"):
            downloader.download_to_temp("invalid-url")


class TestConvenienceFunctions:
    """Tests for convenience functions."""
    
    def test_strict_int(self):
        """Test strict_int function."""
        from strict.core import strict_int
        
        result = strict_int(42)
        assert result == 42
        
        with pytest.raises(StrictError):
            strict_int("42")
    
    def test_strict_str(self):
        """Test strict_str function."""
        from strict.core import strict_str
        
        result = strict_str("hello")
        assert result == "hello"
        
        with pytest.raises(StrictError):
            strict_str(42)
    
    def test_strict_list(self):
        """Test strict_list function."""
        from strict.core import strict_list
        
        result = strict_list([1, 2, 3])
        assert result == [1, 2, 3]
        
        with pytest.raises(StrictError):
            strict_list("not a list")