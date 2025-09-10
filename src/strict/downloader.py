"""
Download functionality for the Strict package.
"""

import os
import tempfile
from pathlib import Path
from typing import Optional, Dict, Any, Union
from urllib.parse import urlparse
import requests

from .core import StrictValidator, StrictError


class StrictDownloader:
    """A strict downloader with validation and safety checks."""
    
    def __init__(self, timeout: int = 30, max_size: int = 100 * 1024 * 1024):  # 100MB default
        self.timeout = timeout
        self.max_size = max_size
        self.validator = StrictValidator()
        self._session = requests.Session()
        self._session.headers.update({
            'User-Agent': 'StrictDownloader/0.1.0'
        })
    
    def validate_url(self, url: str) -> str:
        """Validate and normalize a URL."""
        self.validator.require_type(url, str)
        self.validator.require_non_empty(url)
        
        parsed = urlparse(url)
        if not parsed.scheme or not parsed.netloc:
            raise StrictError(f"Invalid URL format: {url}")
        
        if parsed.scheme not in ('http', 'https'):
            raise StrictError(f"Unsupported URL scheme: {parsed.scheme}")
        
        return url
    
    def validate_file_path(self, file_path: Union[str, Path]) -> Path:
        """Validate and convert file path."""
        if isinstance(file_path, str):
            file_path = Path(file_path)
        
        # Ensure parent directory exists
        file_path.parent.mkdir(parents=True, exist_ok=True)
        
        return file_path
    
    def download_file(self, url: str, destination: Optional[Union[str, Path]] = None, 
                     overwrite: bool = False) -> Path:
        """Download a file from URL to destination."""
        # Validate inputs
        validated_url = self.validate_url(url)
        
        # Determine destination
        if destination is None:
            parsed = urlparse(validated_url)
            filename = os.path.basename(parsed.path) or 'downloaded_file'
            destination = Path.cwd() / filename
        
        dest_path = self.validate_file_path(destination)
        
        # Check if file exists and handle overwrite
        if dest_path.exists() and not overwrite:
            raise StrictError(f"File already exists: {dest_path}")
        
        try:
            # Make request with streaming
            response = self._session.get(validated_url, stream=True, timeout=self.timeout)
            response.raise_for_status()
            
            # Check content length if available
            content_length = response.headers.get('content-length')
            if content_length and int(content_length) > self.max_size:
                raise StrictError(f"File too large: {content_length} bytes (max: {self.max_size})")
            
            # Download with size checking
            downloaded_size = 0
            with open(dest_path, 'wb') as f:
                for chunk in response.iter_content(chunk_size=8192):
                    if chunk:
                        downloaded_size += len(chunk)
                        if downloaded_size > self.max_size:
                            dest_path.unlink()  # Clean up partial file
                            raise StrictError(f"File too large during download: {downloaded_size} bytes")
                        f.write(chunk)
            
            return dest_path
            
        except requests.RequestException as e:
            raise StrictError(f"Download failed: {str(e)}")
    
    def download_to_temp(self, url: str) -> Path:
        """Download a file to a temporary location."""
        validated_url = self.validate_url(url)
        
        # Create temporary file
        temp_dir = Path(tempfile.gettempdir())
        parsed = urlparse(validated_url)
        filename = os.path.basename(parsed.path) or 'temp_download'
        temp_path = temp_dir / f"strict_{filename}"
        
        return self.download_file(validated_url, temp_path, overwrite=True)
    
    def get_file_info(self, url: str) -> Dict[str, Any]:
        """Get information about a file without downloading it."""
        validated_url = self.validate_url(url)
        
        try:
            response = self._session.head(validated_url, timeout=self.timeout)
            response.raise_for_status()
            
            info = {
                'url': validated_url,
                'status_code': response.status_code,
                'headers': dict(response.headers),
                'content_type': response.headers.get('content-type', 'unknown'),
                'content_length': response.headers.get('content-length'),
                'last_modified': response.headers.get('last-modified'),
            }
            
            if info['content_length']:
                info['content_length'] = int(info['content_length'])
            
            return info
            
        except requests.RequestException as e:
            raise StrictError(f"Failed to get file info: {str(e)}")
    
    def close(self):
        """Close the HTTP session."""
        self._session.close()
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()