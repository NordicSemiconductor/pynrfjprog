from setuptools import setup, find_packages
import sys

import pynrfjprog

py2 = sys.version_info[0] == 2
py3 = sys.version_info[0] == 3

if py2:
    requirements = ['enum34', 'future']
elif py3:
    requirements = ['future']


setup(
    
    name = 'pynrfjprog',
        
    version = pynrfjprog.__version__,
    
    description = 'A simple Python interface for the nrfjprog functionality',
    long_description = 'A simple Python interface for the nrfjprog.dll, libnrfjprogdll.so and libnrfjprogdll.dylib. Since the dll, so and dylib are 32-bit applications, this package can only be used with 32-bit Python 2.7.x or 3.4.x or later',
    
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
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6'
    ],
    
    keywords = 'nrfjprog pynrfjprog',
     
    install_requires = requirements,
     
    packages = find_packages(),
    package_data = {
                'pynrfjprog.win_32bit_dll': ['*.dll'],
                'pynrfjprog.win_64bit_dll': ['*.dll'],
                'pynrfjprog.linux_64bit_so': ['*.so'],
                'pynrfjprog.linux_32bit_so': ['*.so'],
                'pynrfjprog.osx_dylib': ['*.dylib'],
                'pynrfjprog.docs': ['*.h', 'nrfjprog_release_notes.txt'],
                'pynrfjprog.examples': ['*.hex']
    }
)
