PaleoDataView

How to Compile:

The source can be compiled by using Qt Creator with MinGW32bit compiler. Just doubleclick the PaleoDataView.pro file and import the project.

To run the compiled code you will need also the following 3rd party resources:
QtXlsx  https://github.com/dbzhang800/QtXlsxWriter
	turn it into one of your Qt-Projects / compile it and add it to your source folder
netCDF 	https://www.unidata.ucar.edu/software/netcdf/
	add the folder to source. it should contain netcdf.dll etc.
Bacon   http://chrono.qub.ac.uk/blaauw/bacon.html   
	add everything to your ../PaleoDataView/Resources/Bacon folder(usually found in Documents)


The available Windows-Installer includes everything necessary and also sets up the working directories.