/*
**	terminfo.h -- Definition of struct term
*/
#	d C1 cur_term->


#d auto_left_margin               C1 Booleans[0]
#d auto_right_margin              C1 Booleans[1]
#d back_color_erase               C1 Booleans[2]
#d can_change                     C1 Booleans[3]
#d ceol_standout_glitch           C1 Booleans[4]
#d col_addr_glitch                C1 Booleans[5]
#d cpi_changes_res                C1 Booleans[6]
#d cr_cancels_micro_mode          C1 Booleans[7]
#d eat_newline_glitch             C1 Booleans[8]
#d erase_overstrike               C1 Booleans[9]
#d generic_type                   C1 Booleans[10]
#d hard_copy                      C1 Booleans[11]
#d hard_cursor                    C1 Booleans[12]
#d has_meta_key                   C1 Booleans[13]
#d has_print_wheel                C1 Booleans[14]
#d has_status_line                C1 Booleans[15]
#d hue_lightness_saturation       C1 Booleans[16]
#d insert_null_glitch             C1 Booleans[17]
#d lpi_changes_res                C1 Booleans[18]
#d memory_above                   C1 Booleans[19]
#d memory_below                   C1 Booleans[20]
#d move_insert_mode               C1 Booleans[21]
#d move_standout_mode             C1 Booleans[22]
#d needs_xon_xoff                 C1 Booleans[23]
#d no_esc_ctlc                    C1 Booleans[24]
#d no_pad_char                    C1 Booleans[25]
#d non_dest_scroll_region         C1 Booleans[26]
#d non_rev_rmcup                  C1 Booleans[27]
#d over_strike                    C1 Booleans[28]
#d prtr_silent                    C1 Booleans[29]
#d row_addr_glitch                C1 Booleans[30]
#d semi_auto_right_margin         C1 Booleans[31]
#d status_line_esc_ok             C1 Booleans[32]
#d dest_tabs_magic_smso           C1 Booleans[33]
#d tilde_glitch                   C1 Booleans[34]
#d transparent_underline          C1 Booleans[35]
#d xon_xoff                       C1 Booleans[36]
#d buffer_capacity                C1 Numbers[0]
#d screen_columns                 C1 Numbers[1]
#d dot_vert_spacing               C1 Numbers[2]
#d dot_horz_spacing               C1 Numbers[3]
#d init_tabs                      C1 Numbers[4]
#d label_height                   C1 Numbers[5]
#d label_width                    C1 Numbers[6]
#d screen_lines                   C1 Numbers[7]
#d lines_of_memory                C1 Numbers[8]
#d magic_cookie_glitch            C1 Numbers[9]
#d max_attributes                 C1 Numbers[10]
#d max_colors                     C1 Numbers[11]
#d max_micro_address              C1 Numbers[12]
#d max_micro_jump                 C1 Numbers[13]
#d max_pairs                      C1 Numbers[14]
#d maximum_windows                C1 Numbers[15]
#d micro_col_size                 C1 Numbers[16]
#d micro_line_size                C1 Numbers[17]
#d no_color_video                 C1 Numbers[18]
#d number_of_pins                 C1 Numbers[19]
#d num_labels                     C1 Numbers[20]
#d output_res_char                C1 Numbers[21]
#d output_res_line                C1 Numbers[22]
#d output_res_horz_inch           C1 Numbers[23]
#d output_res_vert_inch           C1 Numbers[24]
#d padding_baud_rate              C1 Numbers[25]
#d print_rate                     C1 Numbers[26]
#d virtual_terminal               C1 Numbers[27]
#d wide_char_size                 C1 Numbers[28]
#d width_status_line              C1 Numbers[29]
#d acs_chars                      C1 Strings[0]
#d back_tab                       C1 Strings[1]
#d bell                           C1 Strings[2]
#d carriage_return                C1 Strings[3]
#d change_char_pitch              C1 Strings[4]
#d change_line_pitch              C1 Strings[5]
#d change_res_horz                C1 Strings[6]
#d change_res_vert                C1 Strings[7]
#d change_scroll_region           C1 Strings[8]
#d char_padding                   C1 Strings[9]
#d char_set_names                 C1 Strings[10]
#d clear_all_tabs                 C1 Strings[11]
#d clear_margins                  C1 Strings[12]
#d clear_screen                   C1 Strings[13]
#d clr_bol                        C1 Strings[14]
#d clr_eol                        C1 Strings[15]
#d clr_eos                        C1 Strings[16]
#d column_address                 C1 Strings[17]
#d command_character              C1 Strings[18]
#d create_window                  C1 Strings[19]
#d cursor_address                 C1 Strings[20]
#d cursor_down                    C1 Strings[21]
#d cursor_home                    C1 Strings[22]
#d cursor_invisible               C1 Strings[23]
#d cursor_left                    C1 Strings[24]
#d cursor_mem_address             C1 Strings[25]
#d cursor_normal                  C1 Strings[26]
#d cursor_right                   C1 Strings[27]
#d cursor_to_ll                   C1 Strings[28]
#d cursor_up                      C1 Strings[29]
#d cursor_visible                 C1 Strings[30]
#d d_char                    C1 Strings[31]
#d delete_character               C1 Strings[32]
#d delete_line                    C1 Strings[33]
#d delete_phone                   C1 Strings[34]
#d dis_status_line                C1 Strings[35]
#d display_clock                  C1 Strings[36]
#d down_half_line                 C1 Strings[37]
#d ena_acs                        C1 Strings[38]
#d enter_alt_charset_mode         C1 Strings[39]
#d enter_am_mode                  C1 Strings[40]
#d enter_blink_mode               C1 Strings[41]
#d enter_bold_mode                C1 Strings[42]
#d enter_ca_mode                  C1 Strings[43]
#d enter_delete_mode              C1 Strings[44]
#d enter_dim_mode                 C1 Strings[45]
#d enter_doublewide_mode          C1 Strings[46]
#d enter_draft_quality            C1 Strings[47]
#d enter_insert_mode              C1 Strings[48]
#d enter_italics_mode             C1 Strings[49]
#d enter_leftward_mode            C1 Strings[50]
#d enter_micro_mode               C1 Strings[51]
#d enter_near_letter_quality      C1 Strings[52]
#d enter_normal_quality           C1 Strings[53]
#d enter_protected_mode           C1 Strings[54]
#d enter_reverse_mode             C1 Strings[55]
#d enter_secure_mode              C1 Strings[56]
#d enter_shadow_mode              C1 Strings[57]
#d enter_standout_mode            C1 Strings[58]
#d enter_subscript_mode           C1 Strings[59]
#d enter_superscript_mode         C1 Strings[60]
#d enter_underline_mode           C1 Strings[61]
#d enter_upward_mode              C1 Strings[62]
#d enter_xon_mode                 C1 Strings[63]
#d erase_chars                    C1 Strings[64]
#d exit_alt_charset_mode          C1 Strings[65]
#d exit_am_mode                   C1 Strings[66]
#d exit_attribute_mode            C1 Strings[67]
#d exit_ca_mode                   C1 Strings[68]
#d exit_delete_mode               C1 Strings[69]
#d exit_doublewide_mode           C1 Strings[70]
#d exit_insert_mode               C1 Strings[71]
#d exit_italics_mode              C1 Strings[72]
#d exit_leftward_mode             C1 Strings[73]
#d exit_micro_mode                C1 Strings[74]
#d exit_shadow_mode               C1 Strings[75]
#d exit_standout_mode             C1 Strings[76]
#d exit_subscript_mode            C1 Strings[77]
#d exit_superscript_mode          C1 Strings[78]
#d exit_underline_mode            C1 Strings[79]
#d exit_upward_mode               C1 Strings[80]
#d exit_xon_mode                  C1 Strings[81]
#d fixed_pause                    C1 Strings[82]
#d flash_hook                     C1 Strings[83]
#d flash_screen                   C1 Strings[84]
#d form_feed                      C1 Strings[85]
#d from_status_line               C1 Strings[86]
#d goto_window                    C1 Strings[87]
#d hangup                         C1 Strings[88]
#d init_1string                   C1 Strings[89]
#d init_2string                   C1 Strings[90]
#d init_3string                   C1 Strings[91]
#d init_file                      C1 Strings[92]
#d init_prog                      C1 Strings[93]
#d initialize_color               C1 Strings[94]
#d initialize_pair                C1 Strings[95]
#d insert_character               C1 Strings[96]
#d insert_line                    C1 Strings[97]
#d insert_padding                 C1 Strings[98]
#d key_a1                         C1 Strings[99]
#d key_a3                         C1 Strings[100]
#d key_b2                         C1 Strings[101]
#d key_backspace                  C1 Strings[102]
#d key_beg                        C1 Strings[103]
#d key_btab                       C1 Strings[104]
#d key_c1                         C1 Strings[105]
#d key_c3                         C1 Strings[106]
#d key_cancel                     C1 Strings[107]
#d key_catab                      C1 Strings[108]
#d key_clear                      C1 Strings[109]
#d key_close                      C1 Strings[110]
#d key_command                    C1 Strings[111]
#d key_copy                       C1 Strings[112]
#d key_create                     C1 Strings[113]
#d key_ctab                       C1 Strings[114]
#d key_dc                         C1 Strings[115]
#d key_dl                         C1 Strings[116]
#d key_down                       C1 Strings[117]
#d key_eic                        C1 Strings[118]
#d key_end                        C1 Strings[119]
#d key_enter                      C1 Strings[120]
#d key_eol                        C1 Strings[121]
#d key_eos                        C1 Strings[122]
#d key_exit                       C1 Strings[123]
#d key_f0                         C1 Strings[124]
#d key_f1                         C1 Strings[125]
#d key_f2                         C1 Strings[126]
#d key_f3                         C1 Strings[127]
#d key_f4                         C1 Strings[128]
#d key_f5                         C1 Strings[129]
#d key_f6                         C1 Strings[130]
#d key_f7                         C1 Strings[131]
#d key_f8                         C1 Strings[132]
#d key_f9                         C1 Strings[133]
#d key_f10                        C1 Strings[134]
#d key_f11                        C1 Strings[135]
#d key_f12                        C1 Strings[136]
#d key_f13                        C1 Strings[137]
#d key_f14                        C1 Strings[138]
#d key_f15                        C1 Strings[139]
#d key_f16                        C1 Strings[140]
#d key_f17                        C1 Strings[141]
#d key_f18                        C1 Strings[142]
#d key_f19                        C1 Strings[143]
#d key_f20                        C1 Strings[144]
#d key_f21                        C1 Strings[145]
#d key_f22                        C1 Strings[146]
#d key_f23                        C1 Strings[147]
#d key_f24                        C1 Strings[148]
#d key_f25                        C1 Strings[149]
#d key_f26                        C1 Strings[150]
#d key_f27                        C1 Strings[151]
#d key_f28                        C1 Strings[152]
#d key_f29                        C1 Strings[153]
#d key_f30                        C1 Strings[154]
#d key_f31                        C1 Strings[155]
#d key_f32                        C1 Strings[156]
#d key_f33                        C1 Strings[157]
#d key_f34                        C1 Strings[158]
#d key_f35                        C1 Strings[159]
#d key_f36                        C1 Strings[160]
#d key_f37                        C1 Strings[161]
#d key_f38                        C1 Strings[162]
#d key_f39                        C1 Strings[163]
#d key_f40                        C1 Strings[164]
#d key_f41                        C1 Strings[165]
#d key_f42                        C1 Strings[166]
#d key_f43                        C1 Strings[167]
#d key_f44                        C1 Strings[168]
#d key_f45                        C1 Strings[169]
#d key_f46                        C1 Strings[170]
#d key_f47                        C1 Strings[171]
#d key_f48                        C1 Strings[172]
#d key_f49                        C1 Strings[173]
#d key_f50                        C1 Strings[174]
#d key_f51                        C1 Strings[175]
#d key_f52                        C1 Strings[176]
#d key_f53                        C1 Strings[177]
#d key_f54                        C1 Strings[178]
#d key_f55                        C1 Strings[179]
#d key_f56                        C1 Strings[180]
#d key_f57                        C1 Strings[181]
#d key_f58                        C1 Strings[182]
#d key_f59                        C1 Strings[183]
#d key_f60                        C1 Strings[184]
#d key_f61                        C1 Strings[185]
#d key_f62                        C1 Strings[186]
#d key_f63                        C1 Strings[187]
#d key_find                       C1 Strings[188]
#d key_help                       C1 Strings[189]
#d key_home                       C1 Strings[190]
#d key_ic                         C1 Strings[191]
#d key_il                         C1 Strings[192]
#d key_left                       C1 Strings[193]
#d key_ll                         C1 Strings[194]
#d key_mark                       C1 Strings[195]
#d key_message                    C1 Strings[196]
#d key_move                       C1 Strings[197]
#d key_next                       C1 Strings[198]
#d key_npage                      C1 Strings[199]
#d key_open                       C1 Strings[200]
#d key_options                    C1 Strings[201]
#d key_ppage                      C1 Strings[202]
#d key_previous                   C1 Strings[203]
#d key_print                      C1 Strings[204]
#d key_redo                       C1 Strings[205]
#d key_reference                  C1 Strings[206]
#d key_refresh                    C1 Strings[207]
#d key_replace                    C1 Strings[208]
#d key_restart                    C1 Strings[209]
#d key_resume                     C1 Strings[210]
#d key_right                      C1 Strings[211]
#d key_save                       C1 Strings[212]
#d key_sbeg                       C1 Strings[213]
#d key_scancel                    C1 Strings[214]
#d key_scommand                   C1 Strings[215]
#d key_scopy                      C1 Strings[216]
#d key_screate                    C1 Strings[217]
#d key_sdc                        C1 Strings[218]
#d key_sdl                        C1 Strings[219]
#d key_select                     C1 Strings[220]
#d key_send                       C1 Strings[221]
#d key_seol                       C1 Strings[222]
#d key_sexit                      C1 Strings[223]
#d key_sf                         C1 Strings[224]
#d key_sfind                      C1 Strings[225]
#d key_shelp                      C1 Strings[226]
#d key_shome                      C1 Strings[227]
#d key_sic                        C1 Strings[228]
#d key_sleft                      C1 Strings[229]
#d key_smessage                   C1 Strings[230]
#d key_smove                      C1 Strings[231]
#d key_snext                      C1 Strings[232]
#d key_soptions                   C1 Strings[233]
#d key_sprevious                  C1 Strings[234]
#d key_sprint                     C1 Strings[235]
#d key_sr                         C1 Strings[236]
#d key_sredo                      C1 Strings[237]
#d key_sreplace                   C1 Strings[238]
#d key_sright                     C1 Strings[239]
#d key_srsume                     C1 Strings[240]
#d key_ssave                      C1 Strings[241]
#d key_ssuspend                   C1 Strings[242]
#d key_stab                       C1 Strings[243]
#d key_sundo                      C1 Strings[244]
#d key_suspend                    C1 Strings[245]
#d key_undo                       C1 Strings[246]
#d key_up                         C1 Strings[247]
#d keypad_local                   C1 Strings[248]
#d keypad_xmit                    C1 Strings[249]
#d lab_f0                         C1 Strings[250]
#d lab_f1                         C1 Strings[251]
#d lab_f2                         C1 Strings[252]
#d lab_f3                         C1 Strings[253]
#d lab_f4                         C1 Strings[254]
#d lab_f5                         C1 Strings[255]
#d lab_f6                         C1 Strings[256]
#d lab_f7                         C1 Strings[257]
#d lab_f8                         C1 Strings[258]
#d lab_f9                         C1 Strings[259]
#d lab_f10                        C1 Strings[260]
#d label_format                   C1 Strings[261]
#d label_off                      C1 Strings[262]
#d label_on                       C1 Strings[263]
#d meta_off                       C1 Strings[264]
#d meta_on                        C1 Strings[265]
#d micro_column_address           C1 Strings[266]
#d micro_down                     C1 Strings[267]
#d micro_left                     C1 Strings[268]
#d micro_right                    C1 Strings[269]
#d micro_row_address              C1 Strings[270]
#d micro_up                       C1 Strings[271]
#d cursor_newline                 C1 Strings[272]
#d order_of_pins                  C1 Strings[273]
#d orig_colors                    C1 Strings[274]
#d orig_pair                      C1 Strings[275]
#d pad_char                       C1 Strings[276]
#d parm_dch                       C1 Strings[277]
#d parm_delete_line               C1 Strings[278]
#d parm_down_cursor               C1 Strings[279]
#d parm_down_micro                C1 Strings[280]
#d parm_ich                       C1 Strings[281]
#d parm_index                     C1 Strings[282]
#d parm_insert_line               C1 Strings[283]
#d parm_left_cursor               C1 Strings[284]
#d parm_left_micro                C1 Strings[285]
#d parm_right_cursor              C1 Strings[286]
#d parm_right_micro               C1 Strings[287]
#d parm_rindex                    C1 Strings[288]
#d parm_up_cursor                 C1 Strings[289]
#d parm_up_micro                  C1 Strings[290]
#d pkey_key                       C1 Strings[291]
#d pkey_local                     C1 Strings[292]
#d pkey_xmit                      C1 Strings[293]
#d plab_norm                      C1 Strings[294]
#d print_screen                   C1 Strings[295]
#d prtr_non                       C1 Strings[296]
#d prtr_off                       C1 Strings[297]
#d prtr_on                        C1 Strings[298]
#d pulse                          C1 Strings[299]
#d quick_dial                     C1 Strings[300]
#d remove_clock                   C1 Strings[301]
#d repeat_char                    C1 Strings[302]
#d req_for_input                  C1 Strings[303]
#d reset_1string                  C1 Strings[304]
#d reset_2string                  C1 Strings[305]
#d reset_3string                  C1 Strings[306]
#d reset_file                     C1 Strings[307]
#d restore_cursor                 C1 Strings[308]
#d row_address                    C1 Strings[309]
#d save_cursor                    C1 Strings[310]
#d scroll_forward                 C1 Strings[311]
#d scroll_reverse                 C1 Strings[312]
#d select_char_set                C1 Strings[313]
#d set_attributes                 C1 Strings[314]
#d set_background                 C1 Strings[315]
#d set_bottom_margin              C1 Strings[316]
#d set_bottom_margin_parm         C1 Strings[317]
#d set_clock                      C1 Strings[318]
#d set_color_pair                 C1 Strings[319]
#d set_foreground                 C1 Strings[320]
#d set_left_margin                C1 Strings[321]
#d set_left_margin_parm           C1 Strings[322]
#d set_right_margin               C1 Strings[323]
#d set_right_margin_parm          C1 Strings[324]
#d set_tab                        C1 Strings[325]
#d set_top_margin                 C1 Strings[326]
#d set_top_margin_parm            C1 Strings[327]
#d set_window                     C1 Strings[328]
#d start_bit_image                C1 Strings[329]
#d start_char_set_def             C1 Strings[330]
#d stop_bit_image                 C1 Strings[331]
#d stop_char_set_def              C1 Strings[332]
#d subscript_characters           C1 Strings[333]
#d superscript_characters         C1 Strings[334]
#d cursor_tab                     C1 Strings[335]
#d these_cause_cr                 C1 Strings[336]
#d to_status_line                 C1 Strings[337]
#d tone                           C1 Strings[338]
#d underline_char                 C1 Strings[339]
#d up_half_line                   C1 Strings[340]
#d user0                          C1 Strings[341]
#d user1                          C1 Strings[342]
#d user2                          C1 Strings[343]
#d user3                          C1 Strings[344]
#d user4                          C1 Strings[345]
#d user5                          C1 Strings[346]
#d user6                          C1 Strings[347]
#d user7                          C1 Strings[348]
#d user8                          C1 Strings[349]
#d user9                          C1 Strings[350]
#d wait_tone                      C1 Strings[351]
#d xoff_character                 C1 Strings[352]
#d xon_character                  C1 Strings[353]
#d zero_motion                    C1 Strings[354]


typedef struct term
{
   char		 Booleans[37];
   short	 Numbers[30];
   char		 *Strings[355];
} TERMINAL;

TERMINAL	*cur_term;

#d BOOLCOUNT 37
#d NUMCOUNT  30
#d STRCOUNT  355

extern int read_entry(char *, TERMINAL*);
extern int must_swap();