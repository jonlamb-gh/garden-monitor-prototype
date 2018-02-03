/**
 * @file default_config.h
 * @brief TODO.
 *
 */

#ifndef DEFAULT_CONFIG_H
#define DEFAULT_CONFIG_H

#define DEF_ZLOG_CONF_FILE "/etc/zlog/zlog.conf"

#define DEF_AIN_CHANNEL_1143 (5)
#define DEF_AIN_CHANNEL_1127 (4)
#define DEF_AIN_CHANNEL_1125_HUMID (6)
#define DEF_AIN_CHANNEL_1125_TEMP (7)

#define DEF_TIMER_START_DELAY_MS (500UL)
#define DEF_DATA_POLL_INTERVAL_MS (500UL)
#define DEF_GUI_REDRAW_INTERVAL_MS (4000ULL)

// 800 = ~6 hours at 30s intervals
#define DEF_RING_BUFFER_LENGTH (800UL)

#define DEF_SCREEN_WIDTH (800UL)
#define DEF_SCREEN_HEIGHT (480UL)

#define DEF_GRID_OFFSET (11.0f)
#define DEF_GRID_SPACING (25.0f)
#define DEF_GRID_COLOR_ALPHA (0.2f)

#define DEF_COORD_SIZE (4.0f)
#define DEF_COORD_SIZE_LEGEND (10.0f)
#define DEF_COORD_STROKE_WIDTH (0.5f)
#define DEF_COORD_COLOR_ALPHA (0.4f)

#define DEF_PLOT_SCALE_Y_1143 (2.3f)
#define DEF_PLOT_SCALE_Y_1127 (2.6f)
#define DEF_PLOT_SCALE_Y_1125_HUMID (6.0f)
#define DEF_PLOT_SCALE_Y_1125_TEMP (15.0f)

#define DEF_LEGEND_OFFSET_X (25)
#define DEF_LEGEND_OFFSET_Y (40)

#define DEF_LEGEND_ICON_OFFSET (5.0f)

#define DEF_AXIS_OFFSET_X (350.0f)
#define DEF_AXIS_OFFSET_X_DELTA (90.0f)
#define DEF_AXIS_LENGTH_TICK_HALF (10.0f)

#define DEF_AXIS_OFFSET_TICK_1143 (100.0f)
#define DEF_AXIS_OFFSET_TICK_1127 (100.0f)
#define DEF_AXIS_OFFSET_TICK_1125_HUMID (10.0f)
#define DEF_AXIS_OFFSET_TICK_1125_TEMP (5.0f)

#define DEF_FONT_POINT_SIZE (15)
#define DEF_FONT_POINT_SIZE_AXIS_TICK (10)

#endif  /* DEFAULT_CONFIG_H */
