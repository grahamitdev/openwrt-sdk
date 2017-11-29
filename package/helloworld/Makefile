
include $(TOPDIR)/rules.mk

PKG_NAME:=helloworld
PKG_RELEASE:=1
PKG_BASE_NAME:=helloworld

 
 
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)
 
 
include $(INCLUDE_DIR)/package.mk
 
define Package/$(PKG_NAME)
        SECTION:=application
        CATEGORY:=app
	#DEPENDS:=
        TITLE:=helloworld -- prints a snarky message.
endef
 
 
define Package/$(PKG_NAME)/description
        It is walter first package demo.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

 
define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/bin/
endef

 
$(eval $(call BuildPackage,helloworld))
