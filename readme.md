-- SUMMARY --

Eventbrite SBOC allows a Drupal site to interact with the Eventbrite API to retrieve information about specific events and attendees. In its current form, it is labelled and customized for use on the Spelling Bee Of Canada website.

For a full description of the module, visit the project page on GitHub:
 https://github.com/Mark-M3511/Eventbrite_SBOC

To submit bug reports and feature suggestions, or to track changes on GitHub:
  https://github.com/Mark-M3511/Eventbrite_SBOC

-- REQUIREMENTS --

 * File core module
 * Mime Mail
 * Mail System
 * Eventbrite API Access - PHP project at: http://eventbrite.github.com 
 
 -- RECOMMENDATIONS --
 * Ulitamte Cron project - http://drupal.org/project/ultimate_cron

-- INSTALLATION --
  
1) Copy the eventbrite_sboc folder to the modules folder in your installation.

2) Enable the module using Administration -> Modules (/admin/modules).

3) Configure user permissions in Administration -> People, click on the 
   Permissions tab (admin/people/permissions), go to Eventbrite SBOC in the list 
   and check "Configure Eventbrite SBOC".
  
4) Configure Eventbrie SBOC with your credentials and other items at
   (admin/config/system/eventbrite_sboc).
  

