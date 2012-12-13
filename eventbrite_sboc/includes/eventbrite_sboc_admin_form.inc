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
 
 define('EVENTBRITE_SBOC_MODULE_DEBUG_SEND_LIMIT', 5);
 
 function _eventbrite_sboc_config_form($form, &$form_state){ 

	$form['eventbrite_api_settings'] = array(
		'#type' => 'fieldset',
		'#title' => t('Eventbrite API credentials'),
	);
	
	$form['eventbrite_api_settings']['eventbrite_api_notice_1'] = array(
	  '#prefix' => '<p style="color: #F00;"><strong>',
	  '#markup' => t('Please Note: OAUTH Key and User Password fields can be used together or alternately. For security reasons their values are never displayed and will be overwritten each time the form is saved. So, ensure you have those values handy each time.'),
	  '#suffix' => '</strong></p>',
	);
	
	$form['eventbrite_api_settings']['eventbrite_api_key'] = array(
		'#type' => 'textfield',
		'#title' => t('API Key'),
		'#default_value' => check_plain(_eventbrite_sboc_api_key()),
		'#size' => 50,
		'#maxlength' => 250,
		'#description' => t('Enter your Eventbrite api key which grants you access public event information'),
		'#required' => TRUE,
	);
	
	$form['eventbrite_api_settings']['eventbrite_api_user_key'] = array(
		'#type' => 'textfield',
		'#title' => t('API User Key'),
		'#default_value' => check_plain(_eventbrite_sboc_api_user_key()),
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
	
	$form['eventbrite_event_user_credentials'] = array(
		'#type' => 'fieldset',
		'#title' => t('Eventbrite User Credentials'),
	);
	
	$form['eventbrite_event_user_credentials']['eventbrite_sboc_api_user_email'] = array(
		'#type' => 'textfield',
		'#title' => t('User Email'),
		'#default_value' => _eventbrite_sboc_api_user_email(),
		'#size' => 80,
		'#maxlength' => 250,
		'#description' => t('Enter Eventbrite user\'s email (This is the email address associated with their Eventbrite account.)'),
		/* '#required' => TRUE, */
	);
	
	$form['eventbrite_event_user_credentials']['eventbrite_sboc_api_user_password'] = array(
		'#type' => 'password',
		'#title' => t('User Password'),
		'#size' => 50,
		'#maxlength' => 250,
		'#description' => t('Enter Eventbrite user\'s password (This is the password associated with their Eventbrite account.)'),
		/* '#required' => TRUE, */
	);
	
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
		'#description' => t('Enter the event id for the event you wish to retrieve information for'),
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
		'#description' => t('Turn on debug mode'),
	);
	
	$form['eventbrite_app_debug_settings']['eventbrite_sboc_app_full_eb_list_attendees'] = array(
		'#type' => 'checkbox',
		'#title' => t('Debug mode: Display all attendees'),
		'#default_value' => variable_get('eventbrite_sboc_app_full_eb_list_attendees', 0),
		'#description' => t('Debug mode: display full list of attendees fetched from Eventbrite (CAUTION: potentially very slow operation)'),
	);
	
	$options = array();
	for ($ctr = 0; $ctr < EVENTBRITE_SBOC_MODULE_DEBUG_SEND_LIMIT; $ctr++){
	  if ($ctr == 0){
	    $options[] = 'No Limit';
	  }else{
	    $options[] = $ctr; 
	  }
	}
	
	$form['eventbrite_app_debug_settings']['eventbrite_sboc_app_debug_send_limit'] = array(
		'#type' => 'select',
		'#title' => t('Debug mode: Number of emails to send'),
	   /*  '#options' => array(0 => 'No Limit', 1 => '1', 2 => '2', 3 => '3', 4 => '4', 5 => '5'),	 */
        '#options' => $options,	   
		'#default_value' => variable_get('eventbrite_sboc_app_debug_send_limit', 1),
		'#description' => t('Debug mode: Set this to a value other than \'No Limit\' to limit the number of emails that will be processed'),
	); 
	
    return system_settings_form($form);	 
 }
 
 function _eventbrite_sboc_build_email_test_forms(){  
   // return drupal_get_form('_eventbrite_sboc_config_form2') + drupal_get_form('_eventbrite_sboc_config_form3');
   $output = drupal_render(drupal_get_form('_eventbrite_sboc_config_form2'));
   $output .= drupal_render(drupal_get_form('_eventbrite_sboc_config_form3'));
   return $output;
 }

 function _eventbrite_sboc_config_form2($form, &$form_state) {
 
	$form['eventbrite_sboc_file_names'] = array(
		'#type' => 'fieldset',
		'#title' => t('Files to be emailed'),
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

function _eventbrite_sboc_config_form3_submit($form, &$form_state){
   $email_address = $form_state['values']['eventbrite_sboc_email_address'];
   if (valid_email_address($email_address)){
	 _eventbrite_sboc_admin_send_email($email_address);
   }
   else{
     form_set_error('email', t('The e-mail address provided is invalid.')); 
     watch_dog(WATCHDOG_ERROR, format_string('Invalid email address @email_address', array('@email_address' => $email_address)));
   }
}

function _eventbrite_sboc_admin_send_email($email_address) {
   _eventbrite_sboc_get_attendee_list();
   _eventbrite_sboc_process_emails($email_address);
}
 
/*
 * Validate admin settings form
 */

function _eventbrite_sboc_config_form_validate($form, &$form_state){
  $eventbrite_api_key = $form_state['values']['eventbrite_api_key'];
  $eventbrite_api_user_key = $form_state['values']['eventbrite_api_user_key'];
    
  if (empty($eventbrite_app_key) || empty($eventbrite_api_user_key)) {
	return FALSE;
  }
  
  return TRUE;
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