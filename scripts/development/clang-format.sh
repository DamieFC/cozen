#!/bin/sh
cd ../.. && clang-format --style=file -i --verbose $(find kernel/ libraries/ -type f -name '*.c') \
	 && clang-format --style=file -i --verbose $(find kernel/ libraries/ -type f -name '*.h')

