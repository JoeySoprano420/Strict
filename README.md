# Strict

A Python package for strict validation and secure downloading with comprehensive type checking and safety features.

## Features

- **Strict Validation**: Enforce strict type checking and value validation
- **Safe Downloads**: Download files with size limits, timeout controls, and URL validation
- **CLI Interface**: Command-line tools for validation and downloading
- **Type Safety**: Full typing support with type guards and strict enforcement

## Installation

```bash
pip install .
```

For development:
```bash
pip install -e ".[dev]"
```

## Usage

### Python API

#### Strict Validation

```python
from strict import StrictValidator, StrictError

# Create a validator
validator = StrictValidator()

# Type validation
try:
    value = validator.require_type(42, int)  # Returns 42
    validator.require_type("hello", int)     # Raises StrictError
except StrictError as e:
    print(f"Validation failed: {e}")

# Non-null validation
validator.require_not_none("value")  # Returns "value"
validator.require_not_none(None)     # Raises StrictError

# Non-empty validation
validator.require_non_empty("hello")  # Returns "hello"
validator.require_non_empty("")       # Raises StrictError

# Custom rules
validator.add_rule("positive", lambda x: x > 0)
validator.strict_validate("positive", 42)   # Returns 42
validator.strict_validate("positive", -1)   # Raises StrictError
```

#### Safe Downloads

```python
from strict import StrictDownloader

# Create a downloader with safety limits
with StrictDownloader(timeout=30, max_size=100*1024*1024) as downloader:
    # Download a file
    file_path = downloader.download_file(
        "https://example.com/data.json",
        destination="./downloads/data.json",
        overwrite=True
    )
    
    # Get file info without downloading
    info = downloader.get_file_info("https://example.com/data.json")
    print(f"File size: {info['content_length']} bytes")
    
    # Download to temporary location
    temp_path = downloader.download_to_temp("https://example.com/temp-file.txt")
```

### Command Line Interface

#### Download files:
```bash
# Download a file
strict download https://example.com/file.txt -o ./downloads/file.txt

# Get file information only
strict download https://example.com/file.txt --info

# Download with custom limits
strict download https://example.com/file.txt --timeout 60 --max-size 50000000
```

#### Validate values:
```bash
# Validate types
strict validate int 42
strict validate str "hello world"
strict validate nonempty "not empty"
```

## Development

### Running Tests

```bash
pytest tests/
```

### Code Style

```bash
black src/ tests/
flake8 src/ tests/
```

## Safety Features

- **URL validation**: Only HTTP/HTTPS URLs are allowed
- **File size limits**: Configurable maximum download size (default 100MB)
- **Timeout protection**: Configurable request timeouts
- **Path validation**: Safe file path handling with directory creation
- **Type enforcement**: Strict type checking with clear error messages

## License

MIT License