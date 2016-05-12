"""
Tests that can be run without a connected nRF5 device.

Note: We have a large test framework internally that runs our tests on nRF5 devices - yet to be open sourced.
"""

import unittest

from pynrfjprog import API, JLink, MultiAPI

JLINK_DUMMY_PATH = 'DUMMY'

class CITests(unittest.TestCase):
    """
    This class will run pynrfjprog's continuous integration tests.
    
    """

    @classmethod
    def setUpClass(cls):
        pass

    @classmethod
    def tearDownClass(cls):
        pass

    def setUp(self):
        pass

    def tearDown(self):
        pass
    
    def test_can_create_API_instance(self):
        api = API.API('NRF52', jlink_arm_dll_path=JLINK_DUMMY_PATH)
      
    def test_can_create_MultiAPI_instance(self):
        api = MultiAPI.MultiAPI('NRF52', jlink_arm_dll_path=JLINK_DUMMY_PATH)

if __name__ == '__main__':
    """
    Run the tests with specified options.
    """
    unittest.main(verbosity = 2)
