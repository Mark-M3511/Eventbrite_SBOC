<?php
 
 /**
 * Function _eventbrite_sboc_email_distribution_queue
 * @param 
 *	 None
 * @return 
 *	 None
*/
 function _eventbrite_sboc_email_distribution_queue(){
    $alt_email = variable_get('site_mail', _eventbrite_sboc_api_user_email()); 
    $data = new StdClass;
    $data->email = ''; 
    if (variable_get('eventbrite_sboc_email_send_option','test') == 'test'){
       	$data->email = variable_get('eventbrite_sboc_email_return_address', $alt_email);
    }
    $queue = DrupalQueue::get(EVENTBRITE_SBOC_CRON_QUEUE_EMAIL_DISTRIBUTION);
    $queue->createItem($data);
 }
 
/**
 * Function _eventbrite_sboc_email_auto_sender
 *   Cron Queue callback function
 * @param Object data
 *	 Holds custom values passed to the callback function
 * @return 
 *	  None
 */   
function _eventbrite_sboc_email_auto_sender($data){ 
   $active = variable_get('eventbrite_sboc_app_state', 0);	
   if (!$active) {
   	  return 0;  // App state is inactive; go back	
   }
   // Step through the process    
   _eventbrite_sboc_get_pending_list();
   _eventbrite_sboc_process_emails($data->email, EVENTBRITE_SBOC_ATTENDEES_NEW);
   _eventbrite_sboc_process_emails($data->email, EVENTBRITE_SBOC_ATTENDEES_CHANGED);		
   _eventbrite_sboc_clear_session_data();
   
   return 100;
}
  
/**
 * @Email
 * Function _eventbrite_sboc_process_emails
 *   Sends an email to each attendee
 * @param String email
 *	 Recipient's email address
 * @param String dataset_type
 *	 Type of dataset being processed (i.e. new or changed attendee records)
 * @return 
 *	 None
 */
 function _eventbrite_sboc_process_emails($email = '', $dataset_type = EVENTBRITE_SBOC_ATTENDEES_NEW){
   $attendees = (isset($_SESSION[$dataset_type])) ? $_SESSION[$dataset_type] : array();	
   $email = trim($email);  
   if (!isset($_SESSION[EVENTBRITE_SBOC_EMAIL_BODY_1])) {
   	  $_SESSION[EVENTBRITE_SBOC_EMAIL_BODY_1] = _eventbrite_sboc_message_body();
   }
   $content = $_SESSION[EVENTBRITE_SBOC_EMAIL_BODY_1];
   $dataset_type_count = EVENTBRITE_SBOC_ATTENDEES_NEW_COUNT;
   $to = '';
   $ctr = $email_count = 0;
   $debug_limit = variable_get('eventbrite_sboc_app_debug_send_limit', 1);
   $debug_mode = (variable_get('eventbrite_sboc_app_debug', 0) == 1);
   
   if ($dataset_type == EVENTBRITE_SBOC_ATTENDEES_CHANGED){
   	  $dataset_type_count = EVENTBRITE_SBOC_ATTENDEES_CHANGED_COUNT;
   }		

   _eventbrite_sboc_cache_attendee_records_to_db();
   
   foreach($attendees as $a){  
     $replacements = array(
	    '@first_name' => $a->first_name, 
	    '@last_name' => $a->last_name,
        '@category_name' => $a->category_name, 
	    '@extra_message_1' => $a->extra_message_1,
	    '@registration_type' => $a->registration_type,
	    '@child_first_name' => $a->child_first_name,
	    '@child_last_name' => $a->child_last_name,
	    '@region' => $a->region,
	 );   
    
     $message_body = array();
     $message_body[] = format_string($content, $replacements);	
     if ($dataset_type == EVENTBRITE_SBOC_ATTENDEES_CHANGED){
     	$message_body[] = EVENTBRITE_SBOC_EXTRA_MESSAGE_2;
     }
     
	 $to = (empty($email) ? $a->email : $email);
	 
	 $send_mail = TRUE;
	 if ($dataset_type == EVENTBRITE_SBOC_ATTENDEES_CHANGED){
	 	$current_change_date = strtotime($a->changed);
		$dt = _eventbrite_sboc_get_field_value_from_db($_SESSION[EVENTBRITE_SBOC_ATTENDEES_IN_DB], $a, 'change_date');
	 	$prev_change_date = strtotime($dt);
	 	
		$record_changed = $send_mail = ($current_change_date > $prev_change_date);
		$attendees_in_db = &$_SESSION[EVENTBRITE_SBOC_ATTENDEES_IN_DB];
		
		if ($record_changed){
		   $ctr++;
		   $prev_email = _eventbrite_sboc_get_field_value_from_db($attendees_in_db, $a, 'email_address');
		   $prev_category = _eventbrite_sboc_get_field_value_from_db($attendees_in_db, $a, 'category');
		   $prev_notes = _eventbrite_sboc_get_field_value_from_db($attendees_in_db, $a, 'additional_info');
		   $a->email_sent = _eventbrite_sboc_get_field_value_from_db($attendees_in_db, $a, 'email_sent');
		   $a->email_send_date = _eventbrite_sboc_get_field_value_from_db($attendees_in_db, $a, 'email_send_date');
		
		   $data_test = ($a->email != $prev_email || $a->additional_info != $prev_notes);
		
		   if ($a->order_type != EVENTBRITE_SBOC_ORDER_STATUS_PP_COMPLETED){
			  $data_test = ($data_test || ($a->category != $prev_category));
		   }
		
		   $send_mail = ($send_mail && $data_test);
		}			
		// Save anyway if $send_mail == FALSE and $record_changed == TRUE
		if (!$send_mail && $record_changed){
		   _eventbrite_sboc_cache_attendee_records_to_db(FALSE, $a);
		   watchdog(EVENTBRITE_SBOC_MODULE_NAME, 
		   	t('Attendee %id changed. No email sent.'), 
		   	array('%id' => $a->id), WATCHDOG_INFO);
		}
	 } else {
	 	$ctr++;
	 }
	 
	 if ($send_mail){
		$attachments = array();
		$attachments[] = array('attachment' => $a->attachment, 'attachment_name' => drupal_basename($a->attachment),
			'mime_type' => $a->attachment_mime_type,);
		$success = _eventbrite_sboc_send_email($to, $message_body, $attachments);
	 	if ($success){
	 	  $email_count++;	
		  // Update Timestamp 	 	  
		  $a->email_sent = 1;
		  $a->email_send_date = _eventbrite_sboc_get_timestamp();
	      _eventbrite_sboc_cache_attendee_records_to_db(FALSE, $a);
	 	}		
	 }
	 
	 if ($debug_mode){	    
	    if (($debug_limit > 0) && ($email_count >= $debug_limit)){
	    	watchdog(EVENTBRITE_SBOC_MODULE_NAME, 
	    		t('Debug mode is on. Maximum @email_count emails processed.'), 
	    		array('@email_count' => $email_count), WATCHDOG_INFO);
		   break; 
		}
	 }
   
   }   // -- End foreach
   
   if (!isset($_SESSION[EVENTBRITE_SBOC_EMAIL_COUNT])){
   	  $_SESSION[EVENTBRITE_SBOC_EMAIL_COUNT] = 0;	
   }
   $_SESSION[EVENTBRITE_SBOC_EMAIL_COUNT] += $email_count;
   $_SESSION[$dataset_type_count] = $ctr;

   $status = 'UNKNOWN';	
   switch ($dataset_type) {
     case EVENTBRITE_SBOC_ATTENDEES_NEW:
       $status = 'NEW';	
       if (!empty($_SESSION[EVENTBRITE_SBOC_ATTENDEES_TO_DB])){
   	 	  _eventbrite_sboc_insert_attendees_to_db($_SESSION[EVENTBRITE_SBOC_ATTENDEES_TO_DB]); 
   	   }
   	   break;
     case EVENTBRITE_SBOC_ATTENDEES_CHANGED:
       $status = 'CHANGED';
       if (!empty($_SESSION[EVENTBRITE_SBOC_ATTENDEES_TO_DB])){
       	  _eventbrite_sboc_update_attendees_in_db($_SESSION[EVENTBRITE_SBOC_ATTENDEES_TO_DB]);
       }	        	
       break;
   	 default:
   	    break;  	
   }
   watchdog(EVENTBRITE_SBOC_MODULE_NAME, t('@ctr @status record(s) received.'),  
        		array('@ctr' => $ctr, '@status' => $status,), WATCHDOG_INFO);   
   $update_log = TRUE;
   if ($email_count == 0) {
      $update_log = ($debug_mode && (variable_get('eventbrite_sboc_log_zero_counts', 0) == 1));
   }   
   if ($update_log){
   	  watchdog(EVENTBRITE_SBOC_MODULE_NAME, 
   	  	t('Processed @count designated @status attendee record(s). @email_count email(s) sent.'), 
   		array('@count' => $ctr, '@status' => $status, '@email_count' => $email_count,), WATCHDOG_INFO);	
   }
   
   /*
   $alt_email = variable_get('site_mail', _eventbrite_sboc_api_user_email());  
   $to = variable_get('eventbrite_sboc_email_return_address', $alt_email); 
   if (count($attendees) && !empty($to)){
	 $summary = theme('table', _eventbrite_sboc_summary_table_array($attendees));
     $success = _eventbrite_sboc_send_email($to, $summary);
   }
   */
 }
 
 /**
 * @Email
 * Function _eventbrite_sboc_send_email
 *   Sends a single email to a specified address
 * @param String to
 *	 Recipient's email address
 * @param Array message_body
 *	 Array of body text / Default is empty string
 * @param Array attachments
 *	 Array of attachments information (fully qualified filename ['attachment'], attachment name, mime type
 * @return Boolean result
 *	 Returns boolean value indicating success (TRUE) or failure (FALSE)
 * @Reference: http://drupal.stackexchange.com/questions/54996/how-to-use-drupal-mail-to-send-emails-with-custom-template
 */
function _eventbrite_sboc_send_email($to, $message_body = array(), $attachments = array()){    
    $result = FALSE;
    $params = array(); // create parameters array to hold various values for drupal_mail function
    // N.B. Overridden by Mime Mail module from address. Must be explicitly set on that module's config page.
    $params['from'] = variable_get('eventbrite_sboc_email_return_address', ''); 
    $subject_default = variable_get('site_name', EVENTBRITE_SBOC_EMAIL_SUBJECT_DEFAULT);
	$params['subject'] = variable_get('eventbrite_sboc_content_title', $subject_default);
    $params['attachment'] = array();    
	foreach($attachments as $a){
	  $params['attachment'][] = array('attachment' => $a['attachment'], 
	  	'attachment_name' => $a['attachment_name'], 'mime_type' => $a['mime_type'],);
	}
	foreach($message_body as $body){
	  $params['message_body'][] = $body;
	}
	$message_result = array();
    try {
	   if (!valid_email_address($to)){
	      throw new Exception(t('Invalid email address: \'%to\' for recipient', array('%to' => $to)));
	   }
	   if (!valid_email_address($params['from'])){
	      throw new Exception(t('Invalid email address: \'%from\' for sender', array('%from' => $params['from'])));
	   }
       $message_result = drupal_mail(EVENTBRITE_SBOC_MODULE_NAME, EVENTBRITE_SBOC_MAIL_KEY, $to, 
       	language_default(), $params, $params['from']); 
       if (!$message_result['result']){	 	 
	      throw new Exception(t('Email could not be sent to: %to', array('%to' => $to)));
	   }
	   $result = TRUE;
	   watchdog(EVENTBRITE_SBOC_MODULE_NAME, t('Email successfully sent to: %to'), array('%to' => $to), WATCHDOG_INFO); 	 
    }catch(Exception $e){
       watchdog(EVENTBRITE_SBOC_MODULE_NAME, t($e->getMessage()), array(), WATCHDOG_ERROR);
    }
    return $result;
 }
 
 /**
 * @Email
 * Implementation of hook_mail
 * @param String key
 *	 Mail key as defined by the application
 * @param Array &message (PBR)
 *	 Array of system (i.e. Drupal) specific message parameters passed by reference
 * @param Array params
 *	 Array of application specific values to be assigned to the message array
 * @return 
 *	 None
 */
 function eventbrite_sboc_mail($key, &$message, $params){
    switch($key){  
       case EVENTBRITE_SBOC_MAIL_KEY: 
	      $langcode = $message['language']->language;
	      $message['subject'] = check_plain($params['subject']);
		  foreach($params['message_body'] as $body)	{
		  	 $message['body'][] = check_markup($body, 'full_html', $langcode, FALSE);
		  }
	  	  if (!empty($params['attachment'])){
			 foreach($params['attachment'] as $a){
			 	$attachment = array(
		   			'filecontent' => file_get_contents($a['attachment']),	
		   			'filename' => $a['attachment_name'],
		   			'filemime' => $a['mime_type'],
	     	    );
	     	 	$message['params']['attachments'][] = $attachment; 	
	     	 }	
 	  	  }
         break;
       default:
         break; 	 
    }	
 }
 
  /**
 * @Email
 * Function _eventbrite_sboc_extra_messaage_1
 *   Selects extra message text dependent on registration type
 * @param String registration_type
 *	 Registration type (P=Premium; S=Standard)
 * @return String result
 *	 Extra message
 */
 function _eventbrite_sboc_extra_messaage_1($registration_type){
   $result = '';
   switch ($registration_type){
     case 'P':
	   $result = EVENTBRITE_SBOC_EXTRA_MESSAGE_1;
	   break;
	 case 'S':
	   break;
	 default:
	   break;
   }
   return $result;
 }