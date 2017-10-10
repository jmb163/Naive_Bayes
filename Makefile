#!/bin/bash

extrainc="/opt/local/include"
extralib="/opt/local/lib"

main: quicktest.cpp classifier.cpp util.cpp classifier.hpp util.hpp
	g++ -o nbayes -I $(extrainc) -L $(extralib) classifier.cpp util.cpp quicktest.cpp

clean:
	rm -rf nbayes nbayes_config.xml
