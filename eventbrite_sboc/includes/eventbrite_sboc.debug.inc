<?php

 /**
 * @Debug
 * Function _eventbrite_sboc_debug
 *  Calls a user defined function whose name and paramteres are supplied
 * @param String user_func
 *	 Name of user defined function to be called
 * @param Array args
 *	 Array of parameters for user defined function
 * @return
 *	 None
 */
 function _eventbrite_sboc_debug($user_func, $args = array()){
    $debug_mode = variable_get('eventbrite_sboc_app_debug', 0);
    if ($debug_mode){
	  if (function_exists($user_func)){
	    call_user_func_array($user_func, $args);
	  }	
	}
 }
 
 /**
 * @Debug
 * Function _eventbrite_sboc_debug_lists
 *   If debug flag is 1, Devel's dpm() function is called to display values in the browser
 * @param Integer display_list
 *	 Flag indicating whether attendee list should be displayed in the browser
 * @param Array attendees
 *	 Array of attendees
 * @param Array oauth_login_result
 *	 Array of credentials
 * @return
 *	 None
 */
 function _eventbrite_sboc_debug_lists($display_list, $attendees, $oauth_login_result){
    $devel_loaded = module_exists('devel');
    if (!$devel_loaded){
    	$msg = t('Debug is enabled. Install and enable the \'Devel\' module to veiw data being processed by the application.');
		drupal_set_message($msg, 'warning');
		return;
	}
	
    if ($display_list == 1 && (variable_get('eventbrite_sboc_app_debug', 0) == 1)){
       if (variable_get('eventbrite_sboc_app_full_eb_list_attendees', 0) == 1) {	
		  dpm($attendees);
	  }
	  if (variable_get('eventbrite_sboc_app_display_debug_data_rows', 0) == 1){
	    dpm($oauth_login_result);
	    if (empty($_SESSION[EVENTBRITE_SBOC_ATTENDEES_NEW])){
	       $_SESSION[EVENTBRITE_SBOC_ATTENDEES_NEW] = array();
	    }
        dpm($_SESSION[EVENTBRITE_SBOC_ATTENDEES_NEW]);
        
        if (empty($_SESSION[EVENTBRITE_SBOC_ATTENDEES_CHANGED])){
	       $_SESSION[EVENTBRITE_SBOC_ATTENDEES_CHANGED] = array();
	    }
	    dpm($_SESSION[EVENTBRITE_SBOC_ATTENDEES_CHANGED]); 
	    
	    // if (empty($_SESSION[EVENTBRITE_SBOC_ATTENDEES_TO_DB])){
// 	       $_SESSION[EVENTBRITE_SBOC_ATTENDEES_TO_DB] = array();
// 	    }
// 		dpm($_SESSION[EVENTBRITE_SBOC_ATTENDEES_TO_DB]);
		
		if (empty($_SESSION[EVENTBRITE_SBOC_ATTENDEES_IN_DB])){
	       $_SESSION[EVENTBRITE_SBOC_ATTENDEES_IN_DB] = array();
	    }
		dpm($_SESSION[EVENTBRITE_SBOC_ATTENDEES_IN_DB]);
	  }
   }
   
 }
 
 /**
 * @Debug
 * Function _eventbrite_sboc_display_sql
 *   Displays generated sql statement after call to db_select
 * @param Object query
 *	 Generated query object
 * @return
 *	 None
 */
 function _eventbrite_sboc_display_sql($query){
   if (variable_get('eventbrite_sboc_app_show_sql', 0) == 1){
     dpq($query);
   }	 
 }