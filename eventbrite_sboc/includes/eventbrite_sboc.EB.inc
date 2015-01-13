<?php

/**
 * Function _eventbrite_sboc_get_pending_list
 *  Retrieves attendee list from Eventbrite
 * @param Integer display_list
 *	 Flag indicating whether attendee list should be displayed in the browser
 * @return String
 *	 Themed output provided by drupal_render
 * Decode JSON data: get_object_vars(json_decode($json_data));
 */
 function _eventbrite_sboc_get_pending_list($display_list = 0){
   $result = array();  
   $event_id = array('id' => _eventbrite_sboc_event_id());
   
   //  Should ideally be synchronized with the constant EVENTBRITE_SBOC_REGISTRATION_LOW_DATE 
   $offset_days = EVENTBRITE_SBOC_DEFAULT_TIME_SPAN;
   if (variable_get('eventbrite_sboc_app_state', 0) == 0){
   	  $offset_days = ((!empty($_SESSION['pending_list_time_span'])) ? 
   	  		$_SESSION['pending_list_time_span'] : $offset_days);
   }else{
   	  $offset_days = variable_get('eventbrite_sboc_al_time_span', $offset_days);  		
   }
   $offset_days *= -1;
   $start_date = _eventbrite_sboc_get_timestamp("{$offset_days} day");
   watchdog(EVENTBRITE_SBOC_MODULE_NAME, 'Offset Days = @od | Start Date = @start_date', 
   		array('@od' =>  abs($offset_days), '@start_date' => $start_date), WATCHDOG_INFO);
   		
//    $attendee_list_params = array('modified_after' => _eventbrite_sboc_get_timestamp("{$offset_days} day"),); 
//    $attendee_list_params = array('modified_after' => $start_date,);  	
   $attendee_list_params = array(
   	'modified_after' => $start_date,
   	'status' => 'all',
   );  	
   
   $eb_client = NULL;
   
   $auth_key = array();
   if (_eventbrite_sboc_OAuth_credentials($auth_key)){      
	  $eb_client = new Eventbrite($auth_key); 
	  drupal_set_message(t('Using OAUTH'));
   }else{
      $auth_key = _eventbrite_sboc_user_credentials();
	  $eb_client = new Eventbrite($auth_key, $auth_key['user_email'], $auth_key['user_pwd']);
	  drupal_set_message(t('Using UID/Password combination'));
   }
   
   $event_params = array_merge($event_id, $attendee_list_params);
   $eb_attendee_list = new StdClass;
   try{
   	 $eb_attendee_list = $eb_client->event_list_attendees($event_params); 	
     usort($eb_attendee_list->attendees, '_eventbrite_sboc_sort_attendees_by_create_date');
   }catch(Exception $e){
       $eb_attendee_list->attendees = array();
       $values = array('%ebapi' => 'EB API', '%errmsg' => t($e->getMessage()),);
	   watchdog(EVENTBRITE_SBOC_MODULE_NAME, '%ebapi: %errmsg', $values, WATCHDOG_NOTICE);
   } 
   
   _eventbrite_sboc_cache_attendee_list($eb_attendee_list->attendees);   
   
   if (arg(5) != NULL){
     $display_list = (int)arg(5);
   }
      
   $params = array(
     'display_list' => $display_list, 
     'attendees' => $eb_attendee_list->attendees, 
     'oauth_login_result' => $auth_key['oauth_login_result'],
	);	  
	
   _eventbrite_sboc_debug('_eventbrite_sboc_debug_lists', $params); 
   
   $num_attendees = 0;
   try{
      $num_attendees = $eb_client->event_get($event_id)->event->num_attendee_rows;   
   }catch(Exception $e){
      watchdog_exception(EVENTBRITE_SBOC_MODULE_NAME, $e);
   }
   
   
//  $attendee_list = _eventbrite_sboc_display_pending_list($_SESSION[EVENTBRITE_SBOC_ATTENDEES_NEW]);
   $combined_list = array_merge($_SESSION[EVENTBRITE_SBOC_ATTENDEES_NEW], $_SESSION[EVENTBRITE_SBOC_ATTENDEES_CHANGED]);
   $attendee_list = _eventbrite_sboc_display_pending_list( $combined_list);	
   $params = array(
      'attendee_list' => $attendee_list, 
	  'attendee_list_count' => (!empty($num_attendees) ? $num_attendees : 'N/A'),
    );
   
   return theme('attendee_list', $params);
 }

 /**
 * Function _eventbrite_sboc_OAuth_credentials
 *  Submits credentials to Eventbrite API and retreives access token
 * @param Array auth_key
 *	Array of Eventbrite authroization values for OAUTH2 and uid/password methods
 * @@return Boolean result
 *	 True if successful; False otherwise
 */
 function _eventbrite_sboc_OAuth_credentials(&$auth_key){
    $result = FALSE;
    
	try{
	  $auth_key = array(
        'client_secret' => _eventbrite_sboc_oauth_key(),     
	    'access_token' => NULL,
	    'client_id' =>  _eventbrite_sboc_api_key(),
	    'app_key' => _eventbrite_sboc_api_key(),
	    'user_key' => _eventbrite_sboc_api_user_key(),
      );
      $oauth_login_result = Eventbrite::OAuthLogin($auth_key, '_eventbrite_sboc_get_oauth_token', '_eventbrite_sboc_set_oauth_token', '_eventbrite_sboc_del_oauth_token');	  
	  $auth_key['access_token'] = _eventbrite_sboc_get_oauth_token();
	  $auth_key['oauth_login_result'] = $oauth_login_result;
	  $result = ($auth_key['access_token'] != NULL);
	  if (!$result){
	    throw new Exception('Unable to get access token for OAuth authentication');
	  }
	}catch(Exception $e){
	   watchdog(EVENTBRITE_SBOC_MODULE_NAME, t($e->getMessage()), array(), WATCHDOG_ERROR);
	}
	
    return $result;
 }
 
 /**
 * Function _eventbrite_sboc_user_credentials
 *  Retreives uthorization values from database
 * @param 
 *	None
 * @@return Array auth_key
 *	 Returns all credential values received from Evntbrite API call
 */
 function _eventbrite_sboc_user_credentials(){
      
   $auth_key = array(
     'client_secret' => _eventbrite_sboc_oauth_key(),     
	 'client_id' =>  _eventbrite_sboc_api_key(),
	 'app_key' => _eventbrite_sboc_api_key(),
	 'user_key' => _eventbrite_sboc_api_user_key(),
	 'user_email' => _eventbrite_sboc_api_user_email(),
	 'user_pwd' => _eventbrite_sboc_api_user_password(),
    );
	
    return $auth_key;
 }
 
 function _eventbrite_sboc_api_key(){
   return  check_plain(variable_get('eventbrite_api_key', ''));
 }

 function _eventbrite_sboc_api_user_key(){
   return check_plain(variable_get('eventbrite_api_user_key', ''));
 }
 
 function _eventbrite_sboc_oauth_key(){
   return check_plain(variable_get('eventbrite_oauth_key', ''));
 }
 
 function _eventbrite_sboc_api_user_email(){
   return check_plain(variable_get('eventbrite_sboc_api_user_email', ''));
 }
 
 function _eventbrite_sboc_api_user_password(){
   return check_plain(variable_get('eventbrite_sboc_api_user_password', ''));
 }
 
 /**
 * Function _eventbrite_sboc_event_info
 *   Retrieves specific infomation about the Eventbrite event
 * @param 
 *	 None
 * @return String result
 *	 Returns formatted string containing HTML and event data for display in the browser
 */
 function _eventbrite_sboc_event_info(){
   $result = t('No event information available at this time.');  
   $event_id = array('id' => _eventbrite_sboc_event_id()); 
   $eb_client = NULL;
   
   $auth_key = array();
   if (_eventbrite_sboc_OAuth_credentials($auth_key)){      
	  $eb_client = new Eventbrite($auth_key); 
   }else{
      $auth_key = _eventbrite_sboc_user_credentials();
	  $eb_client = new Eventbrite($auth_key, $auth_key['user_email'], $auth_key['user_pwd']);
   }
   
   $eb_event_info = $eb_client->event_get($event_id); 
   $result = t('<strong><a href="@url" target="_blank">@title</a></strong>&nbsp;&nbsp;(@tz)<br />', array('@url'=>$eb_event_info->event->url, '@title'=>$eb_event_info->event->title,'@tz'=>$eb_event_info->event->timezone,));
   $result .= 'Start: ' . date('F j, Y g:i a', strtotime($eb_event_info->event->start_date)) . '<br />';
   $result .= 'End: ' . date('F j, Y g:i a', strtotime($eb_event_info->event->end_date)) . '<br />';
   $result .= 'Capacity: <strong>' . $eb_event_info->event->capacity . '</strong><br />';
   $result .= 'Attendee Count: <strong>' . $eb_event_info->event->num_attendee_rows . '</strong>';
   $result .= '<hr style="width: 100%; border-bottom: 1px solid #C6CDCF;">';
   $pct =  $eb_event_info->event->num_attendee_rows / variable_get('eventbrite_event_registration_target', 100) * 100;
   $pct = round($pct, 1);
   $result .= format_string('Percentage of target: <strong>!pct%</strong> of goal', array('!pct' => $pct)) .'<br />';   
   $pct = $eb_event_info->event->num_attendee_rows / $eb_event_info->event->capacity * 100;
   $pct = round($pct, 1);
   $result .= format_string('Percentage of capacity: <strong>!pct%</strong> of goal', array('!pct' => $pct)) .'<br />';
   return $result;
 }