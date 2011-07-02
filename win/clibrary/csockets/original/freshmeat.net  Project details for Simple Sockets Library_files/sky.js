google_encoding = 'utf8';
google_ad_section = 'default';

if (window.hint) { google_hints = window.hint; }
if (!window.google_ad_client) { google_ad_client = 'ca-ostg_js'; }
if (!window.google_safe) { google_safe = 'high'; }
if (!window.google_max_num_ads) { google_max_num_ads = '4'; }
if (!window.google_ad_output) { google_ad_output = 'js'; }
if (!window.google_feedback) { google_feedback = 'on'; }
if (!window.ostg_sky_width) { ostg_sky_width  = '160'; }
if (!window.ostg_sky_height) { ostg_sky_height  = '600'; }
if (!window.google_image_size) { google_image_size = ostg_sky_width + 'x' + ostg_sky_height; }

/* Originally added to remove HTML ads from Google because of the lack	*/
/* of a handler which was causing display problems. Removed 8.25.08.	*/
//if (window.google_ad_type.match(/html/)) { google_ad_type = google_ad_type.replace(/_html/,""); }

//if (!window.google_ad_type.match(/html/)) { google_ad_type += '_html'; }

if (!window.ostg_style_type) { ostg_style_type = 'standard'; }

if (typeof window.ostg_skip != 'undefined') {
        google_skip = ostg_skip;
} else {
        var ostg_skip = 0;
}

// google_oasu_url = 'http://ad.doubleclick.net/clk;39821406;13502407;y?https://adwords.google.com/select/OnsiteSignupLandingPage?client=ca-ostg_js&referringUrl=sourceforge.net'; 
// google_oasu_url = 'http://ad.doubleclick.net/clk;39821406;13502407;y?https://adwords.google.com/select/OnsiteSignupLandingPage?client=ca-ostg_js&referringUrl=slashdot.org'; 

currWin = window;
//while (currWin.parent != currWin) {
//           currWin = currWin.parent;
//}

// Redirects Google Bot to different pages for content matching
full_url = ""+currWin.location;
if (!window.google_page_url) {
        google_page_url = ""+full_url;
        if (full_url.match(/slashdot/)) {
                google_ad_section = 'apple askslashdot backslash books developers games hardware interviews it linux politics science yro default';
        }
        if (full_url.match(/project\/showfiles\.php\?group_id=(\d+)/)) {
                google_page_url = 'http://sourceforge.net/project/?group_id=' + RegExp.$1;
        }
        if (full_url.match(/project\/downloading\.php\?group_id=(\d+)/)) {
                google_page_url = 'http://sourceforge.net/project/?group_id=' + RegExp.$1;
        }
        if (window.slashdot_article_url) {
                google_page_url = slashdot_article_url;
        }
        if (window.sf_proj_home) { google_page_url = sf_proj_home; }
}

// This function displays the ad results.
// It must be defined above the script that calls show_ads.js
// to guarantee that it is defined when show_ads.js makes the call-back.
function google_ad_request_done(google_ads) {

	// Proceed only if we have ads to display! 
	if (google_ads.length < 1 ) return;

	// Default values
	if(!window.ostg_google_style) { ostg_google_style = 'text-align: center !important; margin: 0 auto;'; }
	if (!window.ostg_ga_style) { ostg_ga_style  = 'font-size:10px; padding: 0 0 1px 0; text-align: center;'; }
	if (!window.ostg_ga_link) { ostg_ga_link = 'font-size:10px;'; }
	if (!window.ostg_ad_sky) { ostg_ad_sky  = 'overflow: hidden; margin: 0 0 5px 0; padding: 0 2px; text-align: left;'; }
	if (!window.ostg_ad_title) { ostg_ad_title  = 'font-weight: bold;'; }
	if (!window.ostg_ad_desc) { ostg_ad_desc  = 'width: 90%; cursor: pointer; color: #555;'; }
	if (!window.ostg_ad_url) { ostg_ad_url  = 'color: #090;font-size:10px;word-wrap: break-word;'; }
	if (!window.ostg_ad_sky_extra) { ostg_ad_sky_extra = ['font-size: 14px;','font-size: 12px;','font-size: 11px;','font-size: 11px;']; }
	if (!window.ostg_ad_title_extra) { ostg_ad_title_extra = ['font-size:16px;', 'font-size:14px;', 'font-size:12px;', 'font-size:12px;']; }
	if (!window.ostg_ad_url_extra) { ostg_ad_url_extra = ['font-size: 14px;', 'font-size:12px;', 'font-size:11px;', 'font-size:11px;']; }
			
        if (ostg_style_type == 'multi_hilite') {

                ostg_google_style = 'font-family: arial, sans-serif; color: #555; overflow: hidden; font-size: 12px; padding: 0;';
                ostg_ga_style = 'font-size: 10px; text-align: center; line-height: 12px;';
                ostg_ga_link = 'font-size:10px; color: #ccc;';
                ostg_ad_sky = 'padding: 5px; margin-right: 5px; float: left; width: 150px; height: 115px; border: 1px solid #fc3; background: #ffe;';
                ostg_ad_title = 'color: #800; font-weight: bold; font-size: 13px;';
                ostg_ad_url = 'color: #888; font-size: 11px;';

                ostg_ad_sky_extra = ['padding: 5px; margin-right: 5px; float: left; width: 150px; height: 115px; border: 1px solid #fc3; background: #ffe;',
                                        'padding: 5px; margin-right: 5px; float: left; width: 150x; height: 115px; border: 1px solid #fc3; background: #ffe;',
                                        'padding: 5px; margin-right: 5px; float: left; width: 150px; height: 115px; border: 1px solid #fc3; background: #ffe;',
                                        'padding: 5px; margin-right: 5px; float: left; width: 150x; height: 115px; border: 1px solid #fc3; background: #ffe;'];
                ostg_ad_title_extra =  ['color: #800; font-weight: bold; font-size: 13px;',
                                        'color: #800; font-weight: bold; font-size: 13px;',
                                        'color: #800; font-weight: bold; font-size: 13px;',
                                        'color: #800; font-weight: bold; font-size: 13px;'];
                ostg_ad_desc = 'cursor: pointer; line-height: 16px;';
                ostg_ad_url_extra = ['color: #888; font-size: 11px;',
                                        'color: #888; font-size: 11px;',
                                        'color: #888; font-size: 11px;',
                                        'color: #888; font-size: 11px;'];

        } else if (ostg_style_type == 'blue') {

                ostg_google_style = 'text-align: center !important; margin: 0 auto;';
                ostg_ga_style  = 'font-size:10px; padding: 0 0 1px 0; text-align: center;';
                ostg_ga_link = 'font-size:10px; color: #ccc';
                ostg_ad_sky  = 'overflow: hidden; width: 150px; height: 200px; background: #def; border: 2px solid #cde; margin: 0 2px 0 0; padding: 4px; text-align: left; float: left;';
                ostg_ad_title = 'font-weight: bold; color: #03c;';
                ostg_ad_url  = 'color: #69f;font-size:10px;';

                ostg_ad_sky_extra = ['width: 150px;font-size: 16px;','width: 150px;font-size: 14px;','width: 150px;font-size: 12px;','width: 150px;font-size: 11px;'];
                ostg_ad_title_extra = ['font-weight: bold; color: #03c;','font-weight: bold; color: #03c;','font-weight: bold; color: #03c;','font-weight: bold; color: #03c;'];
                ostg_ad_desc = 'cursor: pointer; line-height: 16px;';
                ostg_ad_url_extra = ['color: #69f;font-size:10px;','color: #69f;font-size:10px;','color: #69f;font-size:10px;','color: #69f;font-size:10px;'];

        } else if (ostg_style_type == 'itmj') {

                ostg_google_style = 'text-align: left !important; margin: 0 auto;';
                ostg_ga_style  = 'font-size: 12px; padding: 0 0 1px 0; text-align: right;';
                ostg_ga_link = 'font-size:12px; color: #000; font-weight: bold;';
                ostg_ad_sky  = 'overflow: hidden; width: 115px; background: #fff; word-wrap: break-word; font-size: 11px; font-family: arial, sans-serif;';
                ostg_ad_title = 'font-weight: bold; color: #000098;';
                ostg_ad_url  = 'color: #659831; font-size: 10px;';

                ostg_ad_sky_extra = ['width: 115px; font-size: 16px;','width: 115px; font-size: 14px;','width: 115px; font-size: 12px;','width: 115px; font-size: 11px;'];
                ostg_ad_title_extra = ['font-weight: bold; color: #000098;','font-weight: bold; color: #000098;','font-weight: bold; color: #000098;','font-weight: bold; color: #000098;'];
                ostg_ad_desc = 'cursor: pointer; line-height: 16px;';
                ostg_ad_url_extra = ['color: #659831; font-size:10px;','color: #659831; font-size:10px;','color: #659831; font-size:10px;','color: #659831; font-size:10px;'];

        } else if (ostg_style_type == 'linuxcom_grey') {
                
                ostg_google_style = 'text-align: left !important; margin: 0 0 5px 0; padding: 5px;  color: #003399; font-size: 12px; background: #999FAD;';
                ostg_ga_style  = 'font-size: 12px; padding: 5px; text-align: right;'; 
                ostg_ga_link = 'color: #fff; font-size: 12px;';
                ostg_ad_sky_extra = ['width: 115px; font-size: 16px;','width: 115px; font-size: 14px;','width: 115px; font-size: 12px;','width: 115px; font-size: 11px;'];
                ostg_ad_title_extra = ['font-size:16px;', 'font-size:14px;', 'font-size:12px;', 'font-size:12px;']; 
                ostg_ad_desc  = 'width: 100%; cursor: pointer; color: #fff;'; 
                ostg_ad_url_extra = ['color: #fff; font-size: 11px;',
                                        'color: #fff; font-size: 11px;',
                                        'color: #fff; font-size: 11px;',
                                        'color: #fff; font-size: 11px;'];
                
                ostg_ad_url  = 'color: #fff; text-decoration: none;';
                ostg_ad_sky  = 'overflow: hidden; width: 115px; margin: 0 0 5px 0; padding: 0 2px; background: #999FAD;';
                ostg_ad_title = 'color: #3F547F; font-weight: bold;'; 

        } else if (ostg_style_type == 'sourceforge_default') {

		if(!window.ostg_google_style) { ostg_google_style = 'text-align: center !important; margin: 0 auto;'; }
		if (!window.ostg_ga_style) { ostg_ga_style  = 'font-size:10px; padding: 0 0 1px 0; text-align: center;'; }
		if (!window.ostg_ga_link) { ostg_ga_link = 'font-size:10px;'; }
		if (!window.ostg_ad_sky) { ostg_ad_sky  = 'overflow: hidden; margin: 0 0 5px 0; padding: 0 2px; text-align: left;'; }
		if (!window.ostg_ad_title) { ostg_ad_title  = 'font-weight: bold;'; }
		if (!window.ostg_ad_desc) { ostg_ad_desc  = 'width: 90%; cursor: pointer; color: #555;'; }
		if (!window.ostg_ad_url) { ostg_ad_url  = 'color: #090;font-size:10px;word-wrap: break-word;'; }
		if (!window.ostg_ad_sky_extra) { ostg_ad_sky_extra = ['font-size: 16px;','font-size: 14px;','font-size: 12px;','font-size: 11px;']; }
		if (!window.ostg_ad_title_extra) { ostg_ad_title_extra = ['font-size:18px;', 'font-size:14px;', 'font-size:12px;', 'font-size:11px;']; }
		if (!window.ostg_ad_url_extra) { ostg_ad_url_extra = ['font-size: 14px;', 'font-size:12px;', 'font-size:11px;', 'font-size:10px;']; }

	}

	ostg_ad_sky += ostg_ad_sky_extra[google_ads.length - 1];
	ostg_ad_title += ostg_ad_title_extra[google_ads.length - 1];
	ostg_ad_url += ostg_ad_url_extra[google_ads.length - 1];
			
	// Ad shell 
	document.write('<div class="google" style="height:'+ostg_sky_height+'px; width:'+ostg_sky_width+'px;'+ostg_google_style+'">');

	// For text ads, display each ad in turn.
	if (google_ads[0].type == 'text') {
                document.write('<div style="'+ostg_ga_style+'">');
                if (google_info.feedback_url) {
                        document.write('<a target="_blank" href="'+google_info.feedback_url+'" style="'+ostg_ga_link+'">Ads by Google</a>');
                } else {
                        document.write('Ads By Google');
                }
                document.write('</div>');

		for(i = 0; i < google_ads.length; ++i) {
			document.write('<div style="'+ostg_ad_sky+'">');
			document.write('<a href="'+google_ads[i].url+'" target="_blank" style="'+ostg_ad_title+'">'+google_ads[i].line1+'</a><br />');
			document.write('<div style="'+ostg_ad_desc+'" onclick="window.open(\''+google_ads[i].url+'\')">'+google_ads[i].line2+" "+google_ads[i].line3+'</div> ');
			document.write('<a href="'+google_ads[i].url+'" target="_blank" style="'+ostg_ad_url+'">'+google_ads[i].visible_url+'</a>');
			document.write('</div><br />');
		}

		window.ostg_skip += google_ads.length;

	// Image ads
	} else if (google_ads[0].type == 'image') {

		var orig_url = google_ads[0].image_url;
		var final_url = orig_url;
		if (window.location.protocol.match('https')) {
			if(orig_url.match('http:')) {
				final_url = orig_url.replace(/^http:/i, 'https:');
			}
		}
		final_url = final_url.replace(/googlesyndication.com/i, 'googleadservices.com');
		document.write('<a target="_blank" href="'+google_ads[0].url+'"><img src="'+google_ads[0].image_url+'" height="'+google_ads[0].image_height+'" width="'+google_ads[0].image_width+'" border="0" alt="Ad" /></a>');

	// Flash ads
	} else if (google_ads[0].type == "flash") {

		var orig_url = google_ads[0].image_url;
		var final_url = orig_url;
		if (window.location.protocol.match('https')) {
			if(orig_url.match('http:')) {
				final_url = orig_url.replace(/^http:/i, 'https:');
			}
		}
		final_url = final_url.replace(/googlesyndication.com/i, 'googleadservices.com');
		document.write('<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000"' 
			+' codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,0,0"' 
			+' WIDTH="' + google_ads[0].image_width 
			+'" HEIGHT="' + google_ads[0].image_height + '">' 
			+'<PARAM NAME="movie" VALUE="' + google_ads[0].image_url + '">'
			+'<PARAM NAME="quality" VALUE="high">'
			+'<PARAM NAME="AllowScriptAccess" VALUE="never">'
			+'<EMBED src="' + google_ads[0].image_url 
			+'" WIDTH="' + google_ads[0].image_width 
			+'" HEIGHT="' + google_ads[0].image_height 
			+'" TYPE="application/x-shockwave-flash"' 
			+' AllowScriptAccess="never" ' 
			+' PLUGINSPAGE="http://www.macromedia.com/go/getflashplayer"></EMBED></OBJECT>');
	} else if (google_ads[0].type == "html") {
		document.write(google_ads[0].snippet);
	} 

	// Finish up anything that needs finishing up
	document.write ("</div>");

}

