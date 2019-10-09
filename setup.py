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

    name='pynrfjprog',

    version=pynrfjprog.__version__,

    description='A simple Python interface for the nrfjprog functionality',
    long_description='A simple Python interface for the nrfjprog.dll, libnrfjprogdll.so and libnrfjprogdll.dylib. This package can only be used with Python 2.7.x or 3.4.x or later',

    url='http://www.nordicsemi.com/',

    author='Nordic Semiconductor ASA',

    license=open('LICENSE.txt').read(),

    classifiers=[

        'Development Status :: 5 - Production/Stable',

        'Intended Audience :: Developers',

        'Operating System :: MacOS',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: POSIX :: Linux',

        'Topic :: Software Development :: Build Tools',
        'Topic :: Software Development :: Debuggers',
        'Topic :: Software Development :: Embedded Systems',

        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6'
    ],

    keywords='nrfjprog pynrfjprog highlevelpynrfjprog',

    install_requires=requirements,

    packages=find_packages(),
    package_data={
        'pynrfjprog.lib_x86': ['*.dll', '*.so*', '*.dylib*'],
        'pynrfjprog.lib_x64': ['*.dll', '*.so*', '*.dylib*'],
        'pynrfjprog.docs': ['*.h', 'nrfjprog_release_notes.txt'],
        'pynrfjprog.examples': ['*.hex']
    }
)
