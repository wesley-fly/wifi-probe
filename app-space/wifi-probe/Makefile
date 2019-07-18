#
# Top level makefile for example application
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wifi-probe
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/wifi-probe
  SECTION:=utils
  DEPENDS:= +libjson-c
  CATEGORY:=Utilities
  TITLE:= wifi-probe userspace applications
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

TARGET_CFLAGS += $(FPIC)

define Package/wifi-probe/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wifi-probe $(1)/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wifi-report $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,wifi-probe))
