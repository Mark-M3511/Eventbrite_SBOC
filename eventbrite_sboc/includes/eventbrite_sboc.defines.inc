<?php

//  module_load_include('inc', 'eventbrite_sboc', 'includes/eventbrite_sboc_helpers');

 /** defines **/
 define('EVENTBRITE_BASE_URL', 'https://www.eventbrite.com/json/');
 define('EVENTBRITE_SBOC_MAIL_TOKEN', 'eventbrite_sboc_mail_contact');
 define('EVENTBRITE_SBOC_LIST_SORT_ORDER', 'DESC');
 define('EVENTBRITE_SBOC_EMAIL_SEND_LIMIT', 100); 
 /* bof: Question Id's */
 define('EVENTBRITE_SBOC_QUESTION_ID_CHILD_FIRST_NAME', "Contestant's First Name");
 define('EVENTBRITE_SBOC_QUESTION_ID_CHILD_LAST_NAME', "Contestant's Last Name");
 define('EVENTBRITE_SBOC_QUESTION_ID_CATEGORY', "Contestant's Category (b.b. = born between)");
 define('EVENTBRITE_SBOC_QUESTION_ID_REGION', "Contestant's Region");
 define('EVENTBRITE_SBOC_QUESTION_ID_GENDER', "Contestant's Gender");
 define('EVENTBRITE_SBOC_QUESTION_ID_SCHOOL', "Contestant's School");
 define('EVENTBRITE_SBOC_QUESTION_ID_GRADE', "Contestant's Grade");
 define('EVENTBRITE_SBOC_QUESTION_ID_YOB', "Contestant's Year of Birth");
 define('EVENTBRITE_SBOC_QUESTION_ID_MOB', "Contestant's Month Of Birth"); 
 define('EVENTBRITE_SBOC_QUESTION_ID_DOB', "Contestant's Day Of Birth"); 
 define('EVENTBRITE_SBOC_QUESTION_ID_ADDR_LN_1', "Contestant's Address Line 1");  
 define('EVENTBRITE_SBOC_QUESTION_ID_ADDR_LN_2', "Contestant's Address Line 2");   
 define('EVENTBRITE_SBOC_QUESTION_ID_ADDR_CITY', "Contestant's City"); 
 define('EVENTBRITE_SBOC_QUESTION_ID_ADDR_PROV_STATE', "Contestant's Province"); 
 define('EVENTBRITE_SBOC_QUESTION_ID_ADDR_POSTAL_ZIP', "Contestant's Postal Code");  
 define('EVENTBRITE_SBOC_QUESTION_ID_PHONE_1', "Contestant's Phone Number");   
 define('EVENTBRITE_SBOC_QUESTION_ID_PHONE_2', "Contestant's Phone Number 2");   
 define('EVENTBRITE_SBOC_QUESTION_ID_ADDTL_INFO', "Additional information for SBOC");  
 // Email
 define('EVENTBRITE_SBOC_QUESTION_ID_EMAIL_CONSENT', 'I consent to receiving email communications from SBOC');  
 define('EVENTBRITE_SBOC_EMAIL_COUNT', 'email_count');
 define('EVENTBRITE_SBOC_EMAIL_SUBJECT_DEFAULT', 'Registration Welcome');
 define('EVENTBRITE_SBOC_DEFAULT_TIME_SPAN', 30);
 /* eof: Question Id's */
 define('EVENTBRITE_SBOC_REGISTRATION_AMT_STD', 28);
 define('EVENTBRITE_SBOC_REGISTRATION_LOW_DATE', _eventbrite_sboc_get_date('-1 day')); 
 define('EVENTBRITE_SBOC_MAIL_KEY', 'send_manuals');
 define('EVENTBRITE_SBOC_MODULE_NAME', basename(__FILE__,'.module'));
 define('EVENTBRITE_SBOC_API_AUTH_PATH', 'admin/config/system/eventbrite_sboc/api_auth');
//  define('EVENTBRITE_SBOC_EXTRA_MESSAGE_1', ' The coaching and pronunciation CD will be mailed in January');
 define('EVENTBRITE_SBOC_EXTRA_MESSAGE_1', '');
 define('EVENTBRITE_SBOC_EXTRA_MESSAGE_2', 'Please Note: A change was made recently to your registration.');
 define('EVENTBRITE_SBOC_ORDER_STATUS_PP_COMPLETED', 'PayPal Completed');
 define('EVENTBRITE_SBOC_ORDER_STATUS_FREE_ORDER', 'Free Order');
 define('EVENTBRITE_SBOC_PAGE_LIMIT', 10);
 define('EVENTBRITE_SBOC_DATE_SUB_PART', 10);
 // Timezone  
 define('EVENTBRITE_SBOC_DATE_PACIFIC_TZ', 'America/Los Angeles');
 define('EVENTBRITE_SBOC_DATE_EASTERN_TZ', 'America/Toronto');
 // Cron Queue  
 define('EVENTBRITE_SBOC_CRON_QUEUE_EMAIL_DISTRIBUTION', 'cron_queue_eventbrite_sboc_email_distribution');
 // Attendees
 define('EVENTBRITE_SBOC_ATTENDEES_NEW', 'attendees_new');
 define('EVENTBRITE_SBOC_ATTENDEES_CHANGED', 'attendees_changed');
 define('EVENTBRITE_SBOC_ATTENDEES_NEW_COUNT', 'attendees_new_count');
 define('EVENTBRITE_SBOC_ATTENDEES_CHANGED_COUNT', 'attendees_changed_count');
 define('EVENTBRITE_SBOC_ATTENDEES_TO_DB', 'attendees_to_db');
 define('EVENTBRITE_SBOC_ATTENDEES_IN_DB', 'attendees_in_db');
 // menu path
 define('EVENTBRITE_SBOC_MENU_PATH_ADMIN_BASE_PATH', 'admin/config/system/eventbrite_sboc/');
 define('EVENTBRITE_SBOC_MENU_PATH_KICKSERVICE', EVENTBRITE_SBOC_MENU_PATH_ADMIN_BASE_PATH. 'kickservice');
 define('EVENTBRITE_SBOC_MENU_PATH_API_PENDING_LIST', EVENTBRITE_SBOC_MENU_PATH_ADMIN_BASE_PATH. 'api_pending_list');
 define('EVENTBRITE_SBOC_MENU_PATH_DBLOG', 'admin/reports/dblog');
 // Email
 define('EVENTBRITE_SBOC_EMAIL_BODY_1', 'body_content_1');  