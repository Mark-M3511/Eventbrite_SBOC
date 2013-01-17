<?php
/**
 * @file
 * eventbrite_sboc_helpers.inc - contains helper functions used in various parts of the main module files
 *
 */

 function _eventbrite_sboc_api_logout(){
    global $user; 
    _eventbrite_sboc_del_oauth_token();
	if ($user && ($user->uid > 0) && (user_access('administer ebsboc'))){
	   drupal_goto(EVENTBRITE_SBOC_MODULE_API_AUTH_PATH);
	}else{
	   drupal_goto('<front>');
	}
 }
  
 function _eventbrite_sboc_oauth_token(){
    $output = Eventbrite::getAccessToken(); 
	return (!empty($output) ? $output : '');
 } 

 function _eventbrite_sboc_get_oauth_token(){
    return variable_get('eventbrite_sboc_oauth_token', _eventbrite_sboc_oauth_token());
 }
 
 function _eventbrite_sboc_set_oauth_token($eventbrite_sboc_oauth_token){
    return variable_set('eventbrite_sboc_oauth_token', $eventbrite_sboc_oauth_token);
 }
 
 function _eventbrite_sboc_del_oauth_token(){
    variable_del('eventbrite_sboc_oauth_token');
 }

 function _eventbrite_sboc_event_id(){    	
  return variable_get('eventbrite_event_id', '');
 }

 function _eventbrite_sboc_api_key(){
   return  variable_get('eventbrite_api_key', '');
 }

 function _eventbrite_sboc_api_user_key(){
   return variable_get('eventbrite_api_user_key', '');
 }
 
 function _eventbrite_sboc_oauth_key(){
   return variable_get('eventbrite_oauth_key', '');
 }
 
 function _eventbrite_sboc_api_user_email(){
   return variable_get('eventbrite_sboc_api_user_email', '');
 }
 
 function _eventbrite_sboc_api_user_password(){
   return variable_get('eventbrite_sboc_api_user_password', '');
 }
 
 function _eventbrite_sboc_private_files_dir($file_uri){
   return realpath($file_uri);
 }
 
 function _eventbrite_sboc_get_timestamp($offset='now'){
   $tz = new DateTimeZone(_eventbrite_sboc_get_php_safe_tz(date_default_timezone(FALSE)));
   $dt = new DateTime($offset, $tz);
   return $dt->format('Y-m-d H:i:s');
 }
 
 function _eventbrite_sboc_get_date($offset='now'){
   $tz = new DateTimeZone(_eventbrite_sboc_get_php_safe_tz(date_default_timezone(FALSE)));
   $dt = new DateTime($offset, $tz);
   return $dt->format('Y-m-d'); 
 }
 
 function _eventbrite_sboc_get_low_date(){
   if (!isset($_SESSION['pending_list_time_span'])) {
      $_SESSION['pending_list_time_span'] = 1;
   }
   $days = -((int)$_SESSION['pending_list_time_span']);     
   $low_date = _eventbrite_sboc_get_date("$days day"); 
   return $low_date;
 } 
 
 function _eventbrite_sboc_get_date_in_tz($timestamp, $tz_origin, $tz_target=''){
   $current_time = $timestamp;
   try{
     $tz_target = (!empty($tz_target)) ? _eventbrite_sboc_get_php_safe_tz($tz_target) : _eventbrite_sboc_get_php_safe_tz(date_default_timezone(FALSE));
	 $tz_origin = _eventbrite_sboc_get_php_safe_tz($tz_origin);
     $dt = new DateTime($timestamp, new DateTimeZone($tz_origin));
     $dt->setTimezone(new DateTimeZone($tz_target));
	 $current_time = $dt->format('Y-m-d H:i:s');
   }catch(Exception $e){
      watchdog(EVENTBRITE_SBOC_MODULE_NAME, t($e->getMessage()),array(), WATCHDOG_ERROR);
   }
   return $current_time; 
 }
 
 function _eventbrite_sboc_get_timezone_list(){
   $tz_list = timezone_identifiers_list();
   $result = array();
   $dt = new DateTime('now');
   foreach($tz_list as $tz){
	 $dt->setTimezone(new DateTimeZone(_eventbrite_sboc_get_php_safe_tz($tz)));
     $result[] = str_replace('_', ' ', $tz). " [".$dt->format(DateTime::RFC1123). "]"; 
   }
   return $result;
 }
 
 function _eventbrite_sboc_get_timezone_by_index($index){
   $tz_list = timezone_identifiers_list();
   if ($index < 0 ||$index > count($tz_list)){
     $index = 0;
   }
   return _eventbrite_sboc_get_php_safe_tz($tz_list[$index]);
 }
 
 function _eventbrite_sboc_get_php_safe_tz($timezone){
    return str_replace(' ', '_', $timezone);
 }
 
 function _eventbrite_sboc_get_page_num(){
   $qp = drupal_get_query_parameters();
   $page_num = 0;
   if (!empty($qp) && isset($qp['page'])){
      $page_num += (int)$qp['page'];
   }
   return $page_num;
 }

