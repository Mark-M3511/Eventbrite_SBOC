<?php

 /**
 * Function _eventbrite_sboc_cache_attendee_list
 *   Stores attendee records in session variables 
 * @param Integer display_list
 *	 Flag indicating whether attendee list should be displayed in the browser
 * @param Array attendees
 *	 Array of attendees
 * @param Array oauth_login_result
 *	 Array of credentials
 * @return
 *	 None
 */
 function _eventbrite_sboc_cache_attendee_list($attendees){
    $ctr = $pending_ctr = $update_ctr = 0;
	$_SESSION[EVENTBRITE_SBOC_ATTENDEES_NEW] = $_SESSION[EVENTBRITE_SBOC_ATTENDEES_CHANGED] = 
	$_SESSION[EVENTBRITE_SBOC_ATTENDEES_TO_DB] = array();
	$_SESSION[EVENTBRITE_SBOC_ATTENDEES_IN_DB] = _eventbrite_sboc_get_attendees_in_db($attendees);
	/*
	 The values could potentially be used to limit the number of records to be processed. 
	 This may cause issues with older records that have been changed not being processed. 
	 Alternatively, Use available API features of the resepctive databases to limit the number of rows.
	*/
	$low_date = _eventbrite_sboc_get_low_date();
	$tz_name = _eventbrite_sboc_get_timezone_by_index(variable_get('eventbrite_event_registrations_tz', 0));
	$send_limit = variable_get('eventbrite_sboc_email_send_limit', EVENTBRITE_SBOC_EMAIL_SEND_LIMIT);
	$msg = 'Send Limit = @send_limit';
	if ($send_limit == 0){
		$msg .= ' (No Limit!)';
	}
	watchdog(EVENTBRITE_SBOC_MODULE_NAME, $msg, array('@send_limit' => $send_limit), WATCHDOG_INFO);
	
	foreach($attendees as $a){      
	  $newAttendee = new stdClass;
	  $newAttendee->id = $a->attendee->id;
	  $newAttendee->last_name = _eventbrite_sboc_restrict_string_length($a->attendee->last_name, 50);
	  $newAttendee->first_name = _eventbrite_sboc_restrict_string_length($a->attendee->first_name, 50);	  
	  $newAttendee->email = _eventbrite_sboc_restrict_string_length($a->attendee->email, 100);
	  $newAttendee->event_id = $a->attendee->event_id;
	  $newAttendee->order_id = $a->attendee->order_id;			
	  $newAttendee->created = _eventbrite_sboc_get_date_in_tz($a->attendee->created, $tz_name);
	  $newAttendee->changed = _eventbrite_sboc_get_date_in_tz($a->attendee->modified, $tz_name);
	  $newAttendee->amount_paid = $a->attendee->amount_paid;
	  $newAttendee->registration_type = ($a->attendee->amount_paid > EVENTBRITE_SBOC_REGISTRATION_AMT_STD ? 'Premium' : 'Standard');
	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_CATEGORY);
	  $newAttendee->category = $a->attendee->answers[$newAttendee->ans_id]->answer->answer_text[0];
	  $newAttendee->category_name = $a->attendee->answers[$newAttendee->ans_id]->answer->answer_text; 
	  $newAttendee->order_type = $a->attendee->order_type;	 
      $newAttendee->attachment = _eventbrite_sboc_select_attachment($newAttendee->category);		
	  $newAttendee->attachment_mime_type = _eventbrite_sboc_mime_type(pathinfo($newAttendee->attachment, PATHINFO_EXTENSION));
	  $newAttendee->email_sent = 0;
	  $newAttendee->email_send_date = NULL;
	  $newAttendee->extra_message_1 = _eventbrite_sboc_extra_messaage_1($newAttendee->registration_type[0]);
	  $newAttendee->ticket_id = $a->attendee->ticket_id;
	  
	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_CHILD_FIRST_NAME);
	  $newAttendee->child_first_name = _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  $newAttendee->child_first_name = _eventbrite_sboc_restrict_string_length($newAttendee->child_first_name, 50);
	  
	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_CHILD_LAST_NAME);
	  $newAttendee->child_last_name = _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  $newAttendee->child_last_name = _eventbrite_sboc_restrict_string_length($newAttendee->child_last_name, 50);
	  
	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_REGION);
	  $newAttendee->region = _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  $newAttendee->region = _eventbrite_sboc_restrict_string_length($newAttendee->region, 100);
	  
	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_GENDER);
	  $newAttendee->gender = _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  $newAttendee->gender = $newAttendee->gender[0];
	  
	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_SCHOOL);
	  $newAttendee->school = _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  $newAttendee->school = _eventbrite_sboc_restrict_string_length($newAttendee->school, 255);
	  
	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_GRADE);
	  $newAttendee->grade =  _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  
	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_YOB);
	  $newAttendee->yob = _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  
	  /* Select the ordinal value of the month 1 - 12 */
  	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_MOB);
	  $newAttendee->mob = _eventbrite_sboc_month_num(_eventbrite_sboc_answer_text($a, $newAttendee->ans_id));
	  
	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_DOB);
	  $newAttendee->dob = _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  
  	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_ADDR_LN_1);
	  $newAttendee->address_line_1 = _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  $newAttendee->address_line_1 = _eventbrite_sboc_restrict_string_length($newAttendee->address_line_1, 255);
	  
   	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_ADDR_LN_2);
	  $newAttendee->address_line_2 = _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  $newAttendee->address_line_2 = _eventbrite_sboc_restrict_string_length($newAttendee->address_line_2, 255);
	  
   	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_ADDR_CITY);
	  $newAttendee->address_city = _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  $newAttendee->address_city = _eventbrite_sboc_restrict_string_length($newAttendee->address_city, 100);
	  
	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_ADDR_PROV_STATE);
	  $newAttendee->address_prov_state = _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  
 	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_ADDR_POSTAL_ZIP);
	  $newAttendee->address_postal_zip = _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  $newAttendee->address_postal_zip = _eventbrite_sboc_restrict_string_length($newAttendee->address_postal_zip, 20);
	  
	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_PHONE_1);
	  $newAttendee->phone_1 = _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  $newAttendee->phone_1 = _eventbrite_sboc_restrict_string_length($newAttendee->phone_1, 20);
	  
  	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_PHONE_2);
	  $newAttendee->phone_2 = _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  $newAttendee->phone_2 = _eventbrite_sboc_restrict_string_length($newAttendee->phone_2, 20);
	  
	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_ADDTL_INFO);
	  $newAttendee->additional_info = _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  
	  $newAttendee->ans_id = _eventbrite_sboc_answer_id($a->attendee->answers, EVENTBRITE_SBOC_QUESTION_ID_EMAIL_CONSENT);
	  $newAttendee->email_consent = _eventbrite_sboc_answer_text($a, $newAttendee->ans_id);
	  
	  if (_eventbrite_sboc_is_new_attendee($_SESSION[EVENTBRITE_SBOC_ATTENDEES_IN_DB], $a)){  	
	     $_SESSION[EVENTBRITE_SBOC_ATTENDEES_NEW][] = $newAttendee;	
         $pending_ctr++;		
	  }else{	  	
		 $_SESSION[EVENTBRITE_SBOC_ATTENDEES_CHANGED][] = $newAttendee;		
		 $pending_ctr++;		
	  }
	  
	  $ctr++;
	  
	  if ($send_limit > 0 && $ctr >= $send_limit){
	  	 break;
	  }	  
	  
	}	// --eof: foreach

	return $ctr;
 }
 
 /**
 * Function _eventbrite_sboc_new_attendee
 *   Checks whether a record with the identical attendee is in the database
 * @param Array rows
 *  Array of attendee rows from database
 * @param Object attendee
 *	 Attendee record object
 * @return Boolean found
 *	 Flag indicating record does or does not exists
 */
 function _eventbrite_sboc_is_new_attendee($rows, $attendee){
  
   $found = array_key_exists($attendee->attendee->id, $rows);
   
   return (!$found);      
 }