// $Id: README.txt,v 1.2.6.4 2010/11/15 08:07:02 xmattx Exp $

-- SUMMARY --

DownloadFile is a module to direct download files or images.

For a full description of the module, visit the project page:
  http://drupal.org/project/download_file

To submit bug reports and feature suggestions, or to track changes:
  http://drupal.org/project/issues/download_file

-- REQUIREMENTS --

 * File core module

-- INSTALLATION --
  
1) Copy the download_file folder to the modules folder in your installation.

2) Enable the module using Administration -> Modules (/admin/modules).

3) Configure user permissions in Administration -> People, click on the 
   Permissions tab (admin/people/permissions), go to DownloadFile module part 
   and "Access direct download file".
  
4) Manage teaser and full node display settings at Administration -> Structure 
   -> Content types -> "your type" -> manage display 
   (admin/structure/types/manage/"your type"/display).
  
5) Choose a formatter to apply to files or images in that field. Four new 
   formatters "Direct download file" appear in the select list.

6) Configure the format of the link accessible at Administration -> 
   Configuration -> Media -> Download file (/admin/config/media/download-file).

