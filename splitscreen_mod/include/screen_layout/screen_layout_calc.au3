#include-once
#include "screen_layout_db.au3"
#include <Array.au3>

; This returns an array with geometrical data:
;	$ret[0]: x-pos
;	$ret[1]: y-pos
;	$ret[2]: width
;	$ret[3]: height
;
Func screen_layout_calc($screen_w, $screen_h, $border_size_px, _
	$player_count, $variation, $player)

	; Grab everything we need from the "database"
	Local $layout_w		= screen_layout_db($player_count, $variation, "w")
	Local $layout_h		= screen_layout_db($player_count, $variation, "h")
	Local $box_x		= screen_layout_db($player_count, $variation, "x", $player)
	Local $box_y		= screen_layout_db($player_count, $variation, "y", $player)
	Local $box_w		= screen_layout_db($player_count, $variation, "w", $player)
	Local $box_h		= screen_layout_db($player_count, $variation, "h", $player)

	; Calculate the result (TODO: don't always use floor)
	Local $x = Floor($screen_w * $box_x / $layout_w)
	Local $y = Floor($screen_h * $box_y / $layout_h)
	Local $w = Floor($screen_w * $box_w / $layout_w)
	Local $h = Floor($screen_h * $box_h / $layout_h)

	; TODO: Splitscreen border!

	; Give it back
	Local $ret = [ $x, $y, $w, $h ]
	Return $ret
EndFunc

; Returns a $player_res array (see arrays.txt)
Func screen_layout_calc_array($gameinfo)
	Local $player_res[ $gameinfo[0] ]

	; TODO: make these configurable
	Local $screen_w = @DesktopWidth
	Local $screen_h = @DesktopHeight
	Local $border_size = 10 ; not used yet

	For $i = 1 To $gameinfo[0]
		Local $geo = screen_layout_calc($screen_w, $screen_h, $border_size, _
			$gameinfo[0] -1, $gameinfo[1], $i -1)
		$player_res[$i -1] = $geo
	Next

	Return $player_res
EndFunc