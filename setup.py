from platform import version
from setuptools import setup, Extension

setup(name = 'mykmeanssp',
version='1.0',
description='c-api for kmeans hw2',
ext_modules=[Extension('mykmeanssp', sources=['kmeans.c'])])