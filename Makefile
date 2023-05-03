.POSIX:
.SUFFIXES:

PKGNAME=qol-assist
MODULE=github.com/getsolus/qol-assist

MANPAGE=$(PKGNAME).1
SERVICE=$(PKGNAME)-migration.service

VERSION="0.9.0"

PREFIX?=/usr/local
BINDIR?=$(DESTDIR)$(PREFIX)/bin
MANDIR?=$(DESTDIR)$(PREFIX)/man
SYSDIR?=$(DESTDIR)/etc/$(PKGNAME).d
USRDIR?=$(DESTDIR)$(PREFIX)/share/default/$(PKGNAME).d
TRACKDIR?=$(DESTDIR)/var/lib/$(PKGNAME)
SYSTEMDUNITDIR?=$(DESTDIR)/etc/systemd/system
GO?=go
GOFLAGS?=

GOSRC!=find . -name "*.go"
GOSRC+=go.mod go.sum

# Exists in GNUMake but not in NetBSD make and others.
RM?=rm -f

qol-assist: $(GOSRC)
	$(GO) build $(GOFLAGS) \
		-ldflags " \
		-X $(MODULE)/cli.VersionNumber=$(VERSION) \
		-X $(MODULE)/core.TrackDir=$(TRACKDIR) \
		-X $(MODULE)/core.SysDir=$(SYSDIR) \
		-X $(MODULE)/core.UsrDir=$(USRDIR)" \
		-o $@

qol-assist-migration.service: resources/$(SERVICE).in
	cp resources/$(SERVICE).in $(SERVICE)
	sed -i "s:@TRACKDIR@:$(TRACKDIR):g" $(SERVICE)
	sed -i "s:@BINDIR@:$(BINDIR):g" $(SERVICE)

qol-assist.1: resources/$(MANPAGE).scd.in
	scdoc < resources/$(MANPAGE).scd.in > $(MANPAGE)
	sed -i "s:%TRACKDIR%:$(TRACKDIR):g" $(MANPAGE)

all: qol-assist qol-assist-migration.service qol-assist.1

clean:
	$(GO) mod tidy
	$(RM) $(DOCS) $(PKGNAME) $(SERVICE) $(MANPAGE) *.tar.gz
	$(RM) -r vendor

install: all
	install -Dm 0755 $(PKGNAME) $(BINDIR)/$(PKGNAME)
	install -Dm 0644 $(PKGNAME)-migration.service $(SYSTEMDUNITDIR)/$(PKGNAME)-migration.service
	install -Dm 0644 $(MANPAGE) $(MANDIR)/$(MANPAGE)

RMDIR_IF_EMPTY:=sh -c "\
if test -d $$0 && ! ls -1qA $$0 | grep -q . ; then \
	rmdir $$0; \
fi"

uninstall:
	$(RM) $(BINDIR)/$(PKGNAME)
	$(RM) $(SYSTEMDUNITDIR)/$(PKGNAME)-migration.service
	$(RM) $()

check:
	$(GO) get -u github.com/securego/gosec/cmd/gosec
	$(GO) get -u honnef.co/go/tools/cmd/staticcheck
	$(GO) get -u gitlab.com/opennota/check/cmd/aligncheck
	$(GO) fmt -x ./...
	$(GO) vet ./...
	gosec -exclude=G204 ./...
	staticcheck ./...
	aligncheck ./...
	$(GO) test -cover ./...

vendor: check clean
	$(GO) mod vendor

package: vendor
	tar --exclude=".git" \
    		--exclude="*.tar.gz" \
    	       	--exclude="examples" \
    	       	--exclude="tags" \
    	       	--exclude=".vscode" \
    	       	--exclude=".idea" \
    		--exclude="*~" \
    		-zcvf $(PKGNAME)-v$(VERSION).tar.gz ../$(PKGNAME)

.DEFAULT_GOAL := all

.PHONY: all clean install uninstall check vendor package
