
include $(TOPDIR)/rules.mk

PKG_NAME:=zigbeeservice
PKG_RELEASE:=1
PKG_LICENSE:=GPLv2 GPLv2+
PKG_LICENSE_FILES:=
PKG_BUILD_DIR :=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/zigbeeservice
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:= +libmosquitto  +libcyassl +libpthread +librt +libubus +liblua +libblobmsg-json + libuci  +libsqlite3 +libubox +libustream-cyassl +libjson-c +jshn +libopenssl
  TITLE:=  zha gateway
endef

define Package/zigbeeservice/description
 This package is a bridge connectting a Zigbee device through RS-232 serial port
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/zigbeeservice/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/etc/rc.button
	$(CP) $(PKG_BUILD_DIR)/libshuncomzigbee.so $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/zigbeeservice $(1)/usr/bin/
	$(INSTALL_DATA) ./file/iot.crt $(1)/etc/config/
	$(INSTALL_DATA) ./file/cloud $(1)/etc/config/
	$(INSTALL_DATA) ./file/zigbeeserviceS* $(1)/usr/bin/
	$(INSTALL_DATA) ./file/set_ap $(1)/etc/rc.button/
	$(INSTALL_DATA) ./file/set_sta $(1)/etc/rc.button/
	$(INSTALL_BIN) ./file/zha_start $(1)/etc/init.d/zha_start
	$(INSTALL_BIN) ./file/config_status $(1)/etc/init.d/config_status
	$(INSTALL_DATA) ./file/zha_config $(1)/usr/bin/
endef

$(eval $(call BuildPackage,zigbeeservice))
