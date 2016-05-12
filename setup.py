"""
Setup script for pynrfjprog.

USAGE:
    python setup.py install
"""

import os
from setuptools import setup, find_packages
import sys

import pynrfjprog

def read(fname):
    return open(os.path.join(os.path.dirname(__file__), fname)).read()

def read_requirements(fname):
    return open(os.path.join(os.path.dirname(__file__), fname)).readlines()

setup(
    name ='pynrfjprog',
    version = pynrfjprog.__version__,
    description = 'Python wrapper around the nrfjprog DLL',
    long_description=read('README.md'),
    url = 'http://www.nordicsemi.com/',
    author = 'Nordic Semiconductor ASA',
    license = 'BSD',
    classifiers = [
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Operating System :: MacOS',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: POSIX :: Linux',
        'Topic :: Software Development :: Build Tools',
        'Topic :: Software Development :: Debuggers',
        'Topic :: Software Development :: Embedded Systems',
        'License :: OSI Approved :: BSD License',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5'
    ],
    keywords = 'nRF5 nRF51 nRF52 nrfjprog pynrfjprog Nordic Semiconductor SEGGER JLink',
    install_requires = read_requirements('requirements.txt'),
    packages = find_packages(),
    package_data = { 
        'pynrfjprog.win_dll': ['*.dll'],
        'pynrfjprog.linux_64bit_so': ['*.so'],
        'pynrfjprog.linux_32bit_so': ['*.so'],
        'pynrfjprog.osx_dylib': ['*.dylib'],
        'pynrfjprog.docs': ['*.h'],
        'pynrfjprog.examples': ['*.hex'] # TODO: Remove this.
    }
)
