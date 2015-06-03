from setuptools import find_packages, setup


setup_args = dict(
    name='pies',
    version='0.1.0',
    packages=find_packages(),
)

if __name__ == '__main__':
    setup(**setup_args)
