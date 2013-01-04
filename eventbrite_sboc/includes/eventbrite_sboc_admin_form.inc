<?php 

/**
 * @file
 * eventbrite_sboc_admin_form.inc -contains all admin pages, settings, and validate.
 *
 * This file will only be parsed when processing admin menu options.
 * See http://drupal.org/node/146172
 */


/**
 * Settings form for menu callback
 */
 
 require_once(drupal_get_path('module', 'eventbrite_sboc'). '/includes/eventbrite_sboc_helpers.inc');
 
 define('EVENTBRITE_SBOC_MODULE_DEBUG_SEND_LIMIT', 11);
 
 function _eventbrite_sboc_build_config_forms(){ 
	$output = drupal_render(drupal_get_form('_eventbrite_sboc_debug_config_form_api_credentials'));
	$output .= drupal_render(drupal_get_form('_eventbrite_sboc_debug_config_form_client_credentials'));	
	$output .= drupal_render(drupal_get_form('_eventbrite_sboc_debug_config_form_service'));	
	return $output;
 }
 
 function _eventbrite_sboc_debug_config_form_api_credentials($form, &$form_state){
 
 
    $api_key = check_plain(_eventbrite_sboc_api_key());
	$api_user_key = check_plain(_eventbrite_sboc_api_user_key());
 
    $service_status = (variable_get('eventbrite_sboc_app_state', 0) == 0) ? '<span style="color: #F7110A;">Not Active</span>' : '<span style="color: #31C728;">Active</span>';
	$form['service_notifications'] = array(
		'#type' => 'fieldset',
		'#title' => t('Notifications'),
	);
	
	$form['service_notifications']['service_status_notification'] = array(
	  '#prefix' => '<p style="color: #000;"><strong>',
	  '#markup' => t('Service Status: !service_status', array('!service_status' => $service_status)),
	  '#suffix' => '</strong></p>',
	);
	
	/* $dt = new DateTime('now', new DateTimeZone(_eventbrite_sboc_get_php_safe_tz(date_default_timezone(FALSE)))); */
	$current_time = date(DateTime::RFC1123, strtotime(_eventbrite_sboc_get_timestamp()));		
	$current_time_zone = date_default_timezone(FALSE);
	$form['service_notifications']['service_status_date_time'] = array(
	   '#prefix' => '<p style="color: #000;"><strong>',
	   '#markup' => t('Site Time: !current_time&nbsp;|&nbsp;Time Zone: !current_time_zone', array('!current_time' => $current_time, '!current_time_zone' => $current_time_zone)),
	   '#suffix' => '</strong></p>',
	);
	
	$form['service_notifications']['eventbrite_api_notice_1'] = array(
	  '#prefix' => '<p style="color: #000; font-style: italic; border-top: 1px solid #eeeff1; padding-top: 3px;">',
	  '#markup' => t('<strong>Please Note: &nbsp;</strong><span style="text-decoration: underline;">OAUTH Key</span> and <span style="text-decoration: underline;">User Password</span> fields can be used together or alternately. For security reasons their values are never displayed and will be overwritten each time their respective forms are saved. So, ensure you have those values handy each time.'),
	  '#suffix' => '</p>',
	);
 
    $form['eventbrite_api_settings'] = array(
	   '#type' => 'fieldset',
	   '#title' => t('Eventbrite API credentials'),
	   '#collapsible' => TRUE,
       '#collapsed' => (!empty($api_key) && !empty($api_user_key)),	   
	);
		
	
	$form['eventbrite_api_settings']['eventbrite_api_key'] = array(
		'#type' => 'textfield',
		'#title' => t('API Key'),
		'#default_value' => $api_key,
		'#size' => 50,
		'#maxlength' => 250,
		'#description' => t('Enter your Eventbrite api key which grants you access public event information'),
		'#required' => TRUE,
	);
	
	$form['eventbrite_api_settings']['eventbrite_api_user_key'] = array(
		'#type' => 'textfield',
		'#title' => t('API User Key'),
		'#default_value' => $api_user_key,
		'#size' => 50,
		'#maxlength' => 250,
		'#description' => t('Enter your Eventbrite api user key which grants you access to private event information'),
		'#required' => TRUE,
	);
	
	$form['eventbrite_api_settings']['eventbrite_oauth_key'] = array(
		/* '#type' => 'textfield', */
		'#type' => 'password',
		'#title' => t('OAUTH Key'),
		'#size' => 80,
		'#maxlength' => 250,
		'#value_callback' => '_eventbrite_oauth_key_value_callback',
		'#description' => t('Enter your Eventbrite OAUTH key for access authorization'),
	);
    
   return system_settings_form($form);
 }
 
 function _eventbrite_sboc_debug_config_form_client_credentials($form, &$form_state){
 
    $user_email = _eventbrite_sboc_api_user_email();
 
    $form['eventbrite_event_user_credentials'] = array(
	    '#type' => 'fieldset',
	    '#title' => t('Eventbrite User Credentials'),
		'#collapsible' => TRUE,
        '#collapsed' => !empty($user_email),	     
	);
	
	$form['eventbrite_event_user_credentials']['eventbrite_sboc_api_user_email'] = array(
		'#type' => 'textfield',
		'#title' => t('User Email'),
		'#default_value' => $user_email,
		'#size' => 80,
		'#maxlength' => 250,
		'#description' => t('Enter Eventbrite user\'s email (This is the email address associated with their Eventbrite account.)'),
	);
	
	$form['eventbrite_event_user_credentials']['eventbrite_sboc_api_user_password'] = array(
		'#type' => 'password',
		'#title' => t('User Password'),
		'#size' => 50,
		'#maxlength' => 250,
		'#description' => t('Enter Eventbrite user\'s password (This is the password associated with their Eventbrite account.)'),
	);	
	
	return system_settings_form($form); 
 }
 
 function _eventbrite_sboc_debug_config_form_service($form, &$form_state){
 
    $form['eventbrite_event_settings'] = array(
		'#type' => 'fieldset',
		'#title' => t('Eventbrite event information'),
	);
	
	$form['eventbrite_event_settings']['eventbrite_event_id'] = array(
		'#type' => 'textfield',
		'#title' => t('Event Id'),
		'#default_value' => check_plain(_eventbrite_sboc_event_id()),
		'#size' => 50,
		'#maxlength' => 250,
		'#description' => t('Enter the id for the event'),
	);
	
	$form['eventbrite_event_settings']['eventbrite_event_registrations_tz'] = array(
		'#type' => 'select',
		'#title' => t('Attendee Registration Time Zone'),
		'#default_value' => variable_get('eventbrite_event_registrations_tz', 0),
		'#options' => _eventbrite_sboc_get_timezone_list(),
		'#description' => t('Identify the time zone used to timestamp attendee records'),
	);
	
	$form['eventbrite_event_settings']['eventbrite_event_description'] = array(
	  '#prefix' => '<p style="color: #000;">',
	  '#markup' => _eventbrite_sboc_event_info(),
	  '#suffix' => '</p>',
	);
	
	$form['eventbrite_app_settings'] = array(
		'#type' => 'fieldset',
		'#title' => t('Service Settings'),
	);
		
	$form['eventbrite_app_settings']['eventbrite_sboc_app_state'] = array(
		'#type' => 'checkbox',
		'#title' => t('Service is active'),
		'#default_value' => variable_get('eventbrite_sboc_app_state', 0),
		'#description' => t('Check to start service | Uncheck to shut down service'),
	);		
	
    return system_settings_form($form);
 }

 function _eventbrite_sboc_debug_config_form($form, &$form_state){
 
    $form['eventbrite_app_debug_settings'] = array(
		'#type' => 'fieldset',
		'#title' => t('Debug Settings'),
	);
 
    $form['eventbrite_app_debug_settings']['eventbrite_sboc_app_debug'] = array(
		'#type' => 'checkbox',
		'#title' => t('Debug Mode'),
		'#default_value' => variable_get('eventbrite_sboc_app_debug', 0),
		'#description' => t('Turn on/off debug mode'),
	);
	
	$form['eventbrite_app_debug_settings']['eventbrite_sboc_app_display_debug_data_rows'] = array(
		'#type' => 'checkbox',
		'#title' => t('Debug mode: Display data records'),
		'#default_value' => variable_get('eventbrite_sboc_app_display_debug_data_rows', 0),
		'#description' => t('Display data records created during interaction with EB service. (CAUTION: Potentially very slow operation)'),
	);
	
	$form['eventbrite_app_debug_settings']['eventbrite_sboc_app_full_eb_list_attendees'] = array(
		'#type' => 'checkbox',
		'#title' => t('Debug mode: Display full Eventbrite attendee list'),
		'#default_value' => variable_get('eventbrite_sboc_app_full_eb_list_attendees', 0),
		'#description' => t('Display full list of attendees fetched from Eventbrite. (CAUTION: Potentially very slow operation)'),
	);
	
	$options = array('No Limit');
	for ($ctr = 1; $ctr < EVENTBRITE_SBOC_MODULE_DEBUG_SEND_LIMIT; $ctr++){	 
	   $options[] = $ctr; 	
	}

    $option_days = array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	$num_days = array_search((int)$_SESSION['pending_list_time_span'], $option_days);
	$form['eventbrite_app_debug_settings']['eventbrite_sboc_app_debug_pl_time_span'] = array(
		'#type' => 'select',
		'#title' => t('Debug mode: Time span (in days) for pending list'),
        '#options' =>  $option_days,	   
		'#default_value' => ((isset($_SESSION['pending_list_time_span'])) ? $num_days : 0),
		'#description' => t('Select the time span (in days) used to bulid the list of pending attendees. <strong>Please Note:</strong> This setting is session specific.'),
	);  
	
	$form['eventbrite_app_debug_settings']['eventbrite_sboc_app_debug_send_limit'] = array(
		'#type' => 'select',
		'#title' => t('Debug mode: Number of emails to send'),
        '#options' => $options,	   
		'#default_value' => variable_get('eventbrite_sboc_app_debug_send_limit', 1),
		'#description' => t('Select this to a value other than \'No Limit\' to limit the number of emails that will be processed'),
	); 
	
    return system_settings_form($form);	 
 }
 
 function _eventbrite_sboc_debug_set_pl_timespan($form, &$form_state){
    $el = $form['eventbrite_app_debug_settings']['eventbrite_sboc_app_debug_pl_time_span']['#options'];
    $num_days = $el[(int)$form_state['values']['eventbrite_sboc_app_debug_pl_time_span']];
	$_SESSION['pending_list_time_span'] = $num_days;
	drupal_set_message(t('Time span used to select records for \'Pending List\'= !days day(s).', array('!days' => $num_days)));
 }
 
 function _eventbrite_sboc_build_email_test_forms(){  
    $output = drupal_render(drupal_get_form('_eventbrite_sboc_config_form2'));
    $output .= drupal_render(drupal_get_form('_eventbrite_sboc_config_form3'));
    return $output;
 }

 function _eventbrite_sboc_config_form2($form, &$form_state) {
 
	$form['eventbrite_sboc_file_names'] = array(
		'#type' => 'fieldset',
		'#title' => t('Files to be emailed'),
		'#collapsible' => TRUE,
		'#collapsed'   => TRUE,
	);
	
	$form['eventbrite_sboc_file_names']['eventbrite_sboc_filename_1'] = array(
		'#type' => 'textfield',
		'#title' => t('File name (Primary)'),
		'#default_value' => variable_get('eventbrite_sboc_filename_1', ''),
		'#size' => 80,
		'#maxlength' => 250,
		'#description' => t('Enter the name of a file to be emaiiled'),
		'#required' => FALSE,
	);   

	$form['eventbrite_sboc_file_names']['eventbrite_sboc_filename_2'] = array(
		'#type' => 'textfield',
		'#title' => t('File name (Junior)'),
		'#default_value' => variable_get('eventbrite_sboc_filename_2', ''),
		'#size' => 80,
		'#maxlength' => 250,
		'#description' => t('Enter the name of a file to be emaiiled'),
		'#required' => FALSE,
	); 
    
    $form['eventbrite_sboc_file_names']['eventbrite_sboc_filename_3'] = array(
		'#type' => 'textfield',
		'#title' => t('File name (Intermediate)'),
		'#default_value' => variable_get('eventbrite_sboc_filename_3', ''),
		'#size' => 80,
		'#maxlength' => 250,
		'#description' => t('Enter the name of a file to be emaiiled'),
		'#required' => FALSE,
	);  

	$form['eventbrite_sboc_file_node_title'] = array(
		'#type' => 'fieldset',
		'#title' => t('Message Source'),
	);
	
    $form['eventbrite_sboc_file_node_title']['eventbrite_sboc_content_title'] = array(
		'#type' => 'textfield',
		'#title' => t('Enter the title of an existing content type'),
		'#default_value' => variable_get('eventbrite_sboc_content_title', ''),
		'#size' => 100,
		'#maxlength' => 250,
		'#description' => t('Example: Order Confirmation'),
		'#required' => FALSE,
	); 	
	
	$form['eventbrite_sboc_email_info'] = array(
		'#type' => 'fieldset',
		'#title' => t('Email Settings'),
	);
	
	$form['eventbrite_sboc_email_info']['eventbrite_sboc_email_return_address'] = array(
		'#type' => 'textfield',
		'#title' => t('Return address'),
		'#default_value' => variable_get('eventbrite_sboc_email_return_address', ''),
		'#size' => 50,
		'#maxlength' => 100,
		'#description' => t('Enter the address to use for the "from" field; a.k.a the return address.'),
		'#required' => FALSE,
	); 	
	
	$email_options = array('test' => t('Send emails to return address (Test)'), 'live' => t('Send emails to attendees (Live)'),);
	
	$form['eventbrite_sboc_email_info']['eventbrite_sboc_email_send_option'] = array(
      '#type' => 'radios',
      '#title' => t('Send options'),
      '#options' => $email_options,
      '#description' => t('Select the recipient type for email messages'),
	  '#default_value' => _eventbrite_sboc_email_options_index($email_options),
    );
	
	return system_settings_form($form); 
 }
 
 function _eventbrite_sboc_email_options_index($email_options){
   $result = variable_get('eventbrite_sboc_email_send_option','test');  
   return $result; 
 }
 
 function _eventbrite_sboc_config_form3($form, &$form_state){   
    global $user;
	
	$form['eventbrite_sboc_email_test'] = array(
		'#type' => 'fieldset',
		'#title' => t('Send test email'),
	);

    $form['eventbrite_sboc_email_test']['eventbrite_sboc_email_address'] = array(
		'#type' => 'textfield',
		'#title' => t('Email address'),
		'#default_value' => $user->mail,
		'#size' => 50,
		'#maxlength' => 100,
		'#description' => t('Enter the email address to be used'),
		'#required' => TRUE,
	);
	
	$form['eventbrite_sboc_email_test']['eventbrite_sboc_email_send'] = array(
		'#type' => 'submit',
		'#value' => t('Send Email'),		
	);	
	
	return $form;
 }

 function _eventbrite_sboc_config_form3_validate($form, &$form_state){
   $email_address = $form_state['values']['eventbrite_sboc_email_address'];
   if (!valid_email_address($email_address)){
     form_set_error('email', t('The e-mail address provided is invalid.')); 
     watch_dog(WATCHDOG_ERROR, format_string('Invalid email address @email_address', array('@email_address' => $email_address)));
   }
 }
 
 function _eventbrite_sboc_config_form3_submit($form, &$form_state){
    $email_address = $form_state['values']['eventbrite_sboc_email_address'];
    _eventbrite_sboc_admin_send_email($email_address);
 }

 function _eventbrite_sboc_admin_send_email($email_address) {
   _eventbrite_sboc_get_pending_list();
   _eventbrite_sboc_process_emails($email_address);
 }
 
/*
 * Validate admin settings form
 */

 function _eventbrite_sboc_config_form_validate($form, &$form_state){
   $eventbrite_api_key = $form_state['values']['eventbrite_api_key'];
   $eventbrite_api_user_key = $form_state['values']['eventbrite_api_user_key'];
    
   if (empty($eventbrite_app_key) || empty($eventbrite_api_user_key)) {
	 //return FALSE;
	 form_set_error('API Credentials', t('Eventbrite API Key and API Usere Keys are required.'));
   }  
   //return TRUE;
 }

function _eventbrite_sboc_config_form_attendee_report($form, &$form_state){   
	
	$form['eventbrite_sboc_date_select'] = array(
		'#type' => 'fieldset',
		'#title' => t('Select Start and End Dates for report'),
	);

	$end_year = (int)date('Y')+ 1;
	$start_year = $end_year -5;
	for($ctr = $end_year; $ctr >= $start_year; $ctr--) {
	  $years[] =  $ctr;
	} 
	
	$mths = array(1=>'Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec');
	
	$days = array();
	for($ctr = 1; $ctr <= 31; $ctr++) {
	  $days[$ctr] = $ctr;
	}
	
    $form['eventbrite_sboc_date_select']['eventbrite_sboc_year_start'] = array(
		'#type' => 'select',
		'#title' => t('Start Year'),
        '#options' => $years,	   
		'#default_value' => 1,
		'#description' => t('Select Year'),
	);	
	
	$form['eventbrite_sboc_date_select']['eventbrite_sboc_month_start'] = array(
		'#type' => 'select',
		'#title' => t('Start Month'),
        '#options' => $mths,	   
		'#default_value' => (int)date('n', time()),
		'#description' => t('Select Month'),
	);
	
	$form['eventbrite_sboc_date_select']['eventbrite_sboc_day_start'] = array(
		'#type' => 'select',
		'#title' => t('Start Day'),
        '#options' => $days,   
		'#default_value' => (int)date('j', time()),
		'#description' => t('Select Day'),
	);
	
	$form['eventbrite_sboc_date_select']['eventbrite_sboc_year_end'] = array(
		'#type' => 'select',
		'#title' => t('End Year'),
        '#options' => $years,	   
		'#default_value' => 1,
		'#description' => t('Select Year'),
	);	
	
	$form['eventbrite_sboc_date_select']['eventbrite_sboc_month_end'] = array(
		'#type' => 'select',
		'#title' => t('End Month'),
        '#options' => $mths,	   
		'#default_value' => (int)date('n', time()),
		'#description' => t('Select Month'),
	);
	
	$form['eventbrite_sboc_date_select']['eventbrite_sboc_day_end'] = array(
		'#type' => 'select',
		'#title' => t('End Day'),
        '#options' => $days,   
		'#default_value' => (int)date('j', time()),
		'#description' => t('Select Day'),
	);
	
	$form['eventbrite_sboc_date_select']['eventbrite_sboc_submit'] = array(
		'#type' => 'submit',
		'#value' => t('Run Report'),		
	);	
	
	return $form;
 }
 
 function _eventbrite_sboc_config_form_attendee_report_validate($form, &$form_state){
    $start_date = $form['eventbrite_sboc_date_select']['eventbrite_sboc_year_start']['#options'][$form_state['values']['eventbrite_sboc_year_start']];
     $start_date .= '-'. str_pad($form_state['values']['eventbrite_sboc_month_start'],2, '0', STR_PAD_LEFT);
     $start_date .= '-'. str_pad($form_state['values']['eventbrite_sboc_day_start'], 2, '0', STR_PAD_LEFT);
   
    $end_date = $form['eventbrite_sboc_date_select']['eventbrite_sboc_year_end']['#options'][$form_state['values']['eventbrite_sboc_year_end']];
    $end_date .= '-'. str_pad($form_state['values']['eventbrite_sboc_month_end'], 2, '0', STR_PAD_LEFT);
    $end_date .= '-'. str_pad($form_state['values']['eventbrite_sboc_day_end'], 2, '0', STR_PAD_LEFT);
   
    $good_dates = TRUE;
    //9999-99-99
    $good_dates = checkdate((int)substr($start_date,5,2), (int)substr($start_date,8,2), (int)substr($start_date,0,4));
    if (!$good_dates){
      form_set_error('Start Date', t('The start date is invalid: %d', array('%d' => $start_date,)));
    }
    $good_dates = (checkdate((int)substr($end_date,5,2), (int)substr($end_date,8,2), (int)substr($end_date,0,4)));
    if (!$good_dates){
      form_set_error('End Date', t('The end date is invalid: %d', array('%d' => $end_date,))); 
    }     
    if ($good_dates && (strtotime($end_date) < strtotime($start_date))){
      form_set_error('Date Sequence', t('Date sequence is incorrect. Start \'%d1\' later than End \'%d2\'', array('%d1' => $start_date,'%d2' => $end_date,))); 
    }
 }
 
 function _eventbrite_sboc_config_form_attendee_report_submit($form, &$form_state){   
    $start_date = $form['eventbrite_sboc_date_select']['eventbrite_sboc_year_start']['#options'][$form_state['values']['eventbrite_sboc_year_start']];
     $start_date .= '-'. str_pad($form_state['values']['eventbrite_sboc_month_start'],2, '0', STR_PAD_LEFT);
     $start_date .= '-'. str_pad($form_state['values']['eventbrite_sboc_day_start'], 2, '0', STR_PAD_LEFT);
   
    $end_date = $form['eventbrite_sboc_date_select']['eventbrite_sboc_year_end']['#options'][$form_state['values']['eventbrite_sboc_year_end']];
    $end_date .= '-'. str_pad($form_state['values']['eventbrite_sboc_month_end'], 2, '0', STR_PAD_LEFT);
    $end_date .= '-'. str_pad($form_state['values']['eventbrite_sboc_day_end'], 2, '0', STR_PAD_LEFT);
    $form_state['redirect'] = "admin/config/system/eventbrite_sboc/attendee_report/$start_date/$end_date";	
 }

 function _eventbrite_sboc_oauth_login($eventbrite_api_key, $eventbrite_oauth_key){
    global $base_url; 
    $output =  Eventbrite::loginWidget(array( 'app_key' => $eventbrite_api_key,
                                              'client_secret' => $eventbrite_oauth_key,
											  'logout_link' => $base_url . '/eventbrite_sboc/logout',),
											  '_eventbrite_sboc_get_oauth_token', '_eventbrite_sboc_set_oauth_token', '_eventbrite_sboc_del_oauth_token');
	return $output;
 }


 /* function _eventbrite_oauth_key_value_callback($element, $input = FALSE, $form_state = array()){
   $result = ''; 
   if ($input === FALSE){
     $result = _eventbrite_sboc_oauth_key();
   }
   return $result;
 } */