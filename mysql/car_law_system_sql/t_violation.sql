/*
Navicat MySQL Data Transfer

Source Server         : 192.168.1.66_ssyang
Source Server Version : 50549
Source Host           : 192.168.1.66:3306
Source Database       : car_law_system

Target Server Type    : MYSQL
Target Server Version : 50549
File Encoding         : 65001

Date: 2016-05-16 18:56:23
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `t_violation`
-- ----------------------------
DROP TABLE IF EXISTS `t_violation`;
CREATE TABLE `t_violation` (
  `evt_id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `evt_status` tinyint(4) NOT NULL DEFAULT '0',
  `evt_time` timestamp NOT NULL DEFAULT '1970-01-01 08:00:01',
  `evt_location` varchar(128) NOT NULL DEFAULT '',
  `evt_lane_id` tinyint(8) DEFAULT NULL,
  `evt_drive_dir` tinyint(8) DEFAULT NULL,
  `evt_drive_dirext` tinyint(8) DEFAULT NULL,
  `img_wz_0` varchar(256) DEFAULT '',
  `img_wz_1` varchar(256) DEFAULT '',
  `img_wz_2` varchar(256) DEFAULT '',
  `img_plate` varchar(256) DEFAULT '',
  `car_plate` varchar(20) DEFAULT '',
  `car_plate_color` tinyint(8) DEFAULT NULL,
  `car_plate_confidence` smallint(8) DEFAULT NULL,
  `car_type` tinyint(8) DEFAULT NULL,
  `car_color` tinyint(8) DEFAULT NULL,
  `car_logo` int(16) DEFAULT NULL,
  `car_logo_confidence` smallint(8) DEFAULT NULL,
  `car_chexi` int(16) DEFAULT NULL,
  `car_chexi_confidence` smallint(8) DEFAULT NULL,
  `car_chexing` int(16) DEFAULT NULL,
  `car_chexing_confidence` smallint(8) DEFAULT NULL,
  `upload_time` timestamp NULL DEFAULT NULL,
  `update_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`evt_id`,`evt_status`,`evt_time`,`evt_location`)
) ENGINE=InnoDB AUTO_INCREMENT=69 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

