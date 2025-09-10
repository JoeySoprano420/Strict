"""
Core validation functionality for the Strict package.
"""

from typing import Any, Dict, List, Optional, Union, Callable
from typing_extensions import TypeGuard


class StrictError(Exception):
    """Base exception for strict validation errors."""
    pass


class StrictValidator:
    """A strict validator for enforcing type and value constraints."""
    
    def __init__(self):
        self._rules: Dict[str, List[Callable[[Any], bool]]] = {}
    
    def add_rule(self, name: str, validator: Callable[[Any], bool]) -> None:
        """Add a validation rule."""
        if name not in self._rules:
            self._rules[name] = []
        self._rules[name].append(validator)
    
    def validate(self, name: str, value: Any) -> bool:
        """Validate a value against registered rules."""
        if name not in self._rules:
            return True
        
        for rule in self._rules[name]:
            if not rule(value):
                return False
        return True
    
    def strict_validate(self, name: str, value: Any) -> Any:
        """Strictly validate a value, raising an exception on failure."""
        if not self.validate(name, value):
            raise StrictError(f"Validation failed for '{name}' with value: {value}")
        return value
    
    def is_strict_type(self, value: Any, expected_type: type) -> TypeGuard[Any]:
        """Type guard for strict type checking."""
        return isinstance(value, expected_type)
    
    def require_type(self, value: Any, expected_type: type) -> Any:
        """Require a value to be of a specific type."""
        if not isinstance(value, expected_type):
            raise StrictError(
                f"Expected type {expected_type.__name__}, got {type(value).__name__}"
            )
        return value
    
    def require_not_none(self, value: Any) -> Any:
        """Require a value to not be None."""
        if value is None:
            raise StrictError("Value cannot be None")
        return value
    
    def require_non_empty(self, value: Union[str, List, Dict]) -> Union[str, List, Dict]:
        """Require a collection to be non-empty."""
        if not value:
            raise StrictError("Value cannot be empty")
        return value


# Convenience functions for common validations
def strict_int(value: Any) -> int:
    """Strictly convert to int."""
    validator = StrictValidator()
    return validator.require_type(value, int)


def strict_str(value: Any) -> str:
    """Strictly convert to str."""
    validator = StrictValidator()
    return validator.require_type(value, str)


def strict_list(value: Any) -> list:
    """Strictly convert to list."""
    validator = StrictValidator()
    return validator.require_type(value, list)