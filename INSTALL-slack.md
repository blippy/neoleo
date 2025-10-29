My personal notes for creating an sbopkg for slackbuilds
Created 2023-09-11

Guide to packaging:
https://www.slackbuilds.org/guidelines/

As of 2023-09-11, this apparently fails to build for 32-bit Slackware 15.0.

One-time only: 
	install sbo-maintainer-tools from sbopkg, which contains 
	sbolint and sbopkglint

Package up the sbopkg:
	tar cvfz neoleo.tar.gz neoleo

Check that the sbopkg is correct:
	sbolint neoleo.tar.gz

