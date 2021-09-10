from setuptools import setup, find_packages
import pynrfjprog

setup(

    name='pynrfjprog',

    version=pynrfjprog.__version__,

    description='A simple Python interface for the nrfjprog functionality',
    long_description='A simple Python interface for the nrfjprog.dll, libnrfjprogdll.so and libnrfjprogdll.dylib. This package can only be used with Python 2.7.x or 3.4.x or later',

    url='http://www.nordicsemi.com/',

    author='Nordic Semiconductor ASA',
    author_email='sagtools@nordicsemi.no',

    license=open('LICENSE').read(),

    python_requires='>=3.5',

    classifiers=[

        'Development Status :: 5 - Production/Stable',

        'Intended Audience :: Developers',

        'Operating System :: MacOS',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: POSIX :: Linux',

        'Topic :: Software Development :: Build Tools',
        'Topic :: Software Development :: Debuggers',
        'Topic :: Software Development :: Embedded Systems',

        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9'
    ],

    keywords='nrfjprog pynrfjprog highlevelpynrfjprog',

    install_requires=['future'],

    packages=find_packages(),
    package_data={
        'pynrfjprog.lib_x86': ['*.dll', '*.so*', '*.dylib*', 'jlinkarm_nrf_worker*'],
        'pynrfjprog.lib_x64': ['*.dll', '*.so*', '*.dylib*', 'jlinkarm_nrf_worker*'],
        'pynrfjprog.docs': ['*.h', 'nrfjprog_release_notes*.txt'],
        'pynrfjprog.examples': ['*.hex']
    }
)
