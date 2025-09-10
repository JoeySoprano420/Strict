"""
Strict - A validation and download utility package.
"""

__version__ = "0.1.0"
__author__ = "JoeySoprano420"

from .core import StrictValidator, StrictError
from .downloader import StrictDownloader

__all__ = ["StrictValidator", "StrictError", "StrictDownloader"]