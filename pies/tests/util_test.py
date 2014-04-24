"""
Tests for `pies.util`.
"""
import unittest
from pies import util

class PiesTestCase(unittest.TestCase):
    """
    Base tests for the `pies.util` script.
    """
    def test_no_crash(self):
        """
        We should be able to say hello and not crash
        """
        ret = util.say_hello()
        self.assertIsNone(ret)
