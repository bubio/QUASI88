extern byte menu_cursor_on[];
extern byte menu_cursor_off[];
extern int  menu_cursor_x;
extern int  menu_cursor_y;

/******************************************************************************
 * メニューモードの画面描画
 *****************************************************************************/
#ifdef MENU2SCREEN
int    MENU2SCREEN(T_RECTANGLE *updated_area)
{
	int x, y;
	T_Q8GR_TVRAM *old = &q8gr_tvram[q8gr_tvram_flip ^ 1][MENU_Y][MENU_X];
	T_Q8GR_TVRAM *src = &q8gr_tvram[q8gr_tvram_flip    ][MENU_Y][MENU_X];
	TYPE                *dst = (TYPE *) MENU_START;
	int x0 = MENU_W - 1, x1 = 0,  y0 = MENU_H - 1, y1 = 0;

	/*menu_cursor_x = menu_cursor_y = -1;*/

	for (y = 0; y < MENU_H; y++) {
		for (x = 0; x < MENU_W; x++) {

			if (*((Uint *) src) != *((Uint *) old)) {

				int   i, font_inc, font_dup, font_skip;
				byte  style, *font_ptr;
				int   reverse   = (src->reverse) ? 0xff : 0x00;
				int   underline =  src->underline;
				TYPE  fg        = MENU_COLOR_PIXEL(src->foreground);
				TYPE  bg        = MENU_COLOR_PIXEL(src->background);
				byte *cur_ptr = (src->mouse) ? menu_cursor_on : menu_cursor_off;
				WORK_DEFINE();

				/*if (src->mouse) { menu_cursor_x = x; menu_cursor_y = y; }*/

				if (y < y0) {
					y0 = y;
				}
				if (y > y1) {
					y1 = y;
				}
				if (x < x0) {
					x0 = x;
				}
				if (x > x1) {
					x1 = x;
				}

				switch (src->font_type) {
				case FONT_ANK:
					font_ptr = &font_mem[ src->addr ];
					FONT_8x8();
					break;
				case FONT_QUART:
					font_ptr = &kanji_rom[0][ src->addr ][0];
					FONT_8x8();
					break;
				case FONT_HALF:
					font_ptr = &kanji_rom[0][ src->addr ][0];
					FONT_8x16();
					break;
				case FONT_KNJ1L:
					font_ptr = &kanji_rom[0][ src->addr ][0];
					FONT_16x16();
					break;
				case FONT_KNJ1R:
					font_ptr = &kanji_rom[0][ src->addr ][0];
					font_ptr ++;
					FONT_16x16();
					break;
				case FONT_KNJ2L:
					font_ptr = &kanji_rom[1][ src->addr ][0];
					FONT_16x16();
					break;
				case FONT_KNJ2R:
					font_ptr = &kanji_rom[1][ src->addr ][1];
					FONT_16x16();
					break;
				case FONT_KNJXL:
					font_ptr = &kanji_dummy_rom[0][0];
					FONT_16x16();
					break;
				case FONT_KNJXR:
					font_ptr = &kanji_dummy_rom[0][1];
					FONT_16x16();
					break;
				case FONT_BMP:
					font_ptr =
						bitmap_table[ src->addr >> BITMAP_DATA_MAX_OF_BIT ]
						+ (src->addr & ((1 << BITMAP_DATA_MAX_OF_BIT) - 1));
					FONT_8x16();
					break;
				default:        /* trap */
					font_ptr = &font_mem[0];
					FONT_8x8();
					break;
				}

				for (i = 16; i; i -= font_skip) {
					style = GET_FONT() ^ reverse;
					if (i <= 2 && underline) {
						style = 0xff;
					}
					style ^= GET_CURSOR();

					PUT_FONT()

					if (font_dup == FALSE || (i & 1)) {
						font_ptr += font_inc;
					}
					cur_ptr += font_skip;
				}
				dst = dst - FONT_H * SCREEN_WIDTH;

			}
			old ++;
			src ++;
			dst += FONT_W;
		}
		dst += FONT_H * SCREEN_WIDTH - FONT_W * MENU_W;
	}

	switch (MENU_DECORATE) {
	case 0:
		break;
	case 1:
		dst -= SCREEN_WIDTH;
		for (x = 0; x < FONT_W * MENU_W; x++) {
			*dst++ = BLACK;
		}
		break;
	case 2:
		dst -= FONT_H * SCREEN_WIDTH;
		for (x = 0; x < FONT_W * MENU_W; x++) {
			*dst++ = BLACK;
		}
		break;
	}

	if (x0 <= x1) {
		if (updated_area) {
			updated_area->x0 = ((x0) * 1) * 8;
			updated_area->y0 = ((y0) * 8) * 2;
			updated_area->x1 = ((x1 + 1) * 1) * 8;
			updated_area->y1 = ((y1 + 1) * 8) * 2;
		}
		return TRUE;
	} else {
		return FALSE;
	}
}
#endif /* MENU2SCREEN */

/******************************************************************************
 *
 *****************************************************************************/
#undef FONT_H
#undef FONT_W
#undef FONT_8x8
#undef FONT_8x16
#undef FONT_16x16
#undef WORK_DEFINE
#undef GET_FONT
#undef GET_CURSOR
#undef PUT_FONT
#undef MENU2SCREEN
#undef MENU_W
#undef MENU_H
#undef MENU_X
#undef MENU_Y
#undef MENU_START
#undef MENU_DECORATE
