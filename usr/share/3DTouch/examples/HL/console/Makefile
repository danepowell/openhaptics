# Makefile - ConsoleExamples

.PHONY: all
all: \
	CannedForceEffect \
	CustomForceEffect \
	CustomShape \
	Deployment \
	EffectAttributes

.PHONY: CannedForceEffect
CannedForceEffect:
	$(MAKE) -C CannedForceEffect

.PHONY: CustomForceEffect
CustomForceEffect:
	$(MAKE) -C CustomForceEffect

.PHONY: CustomShape
CustomShape:
	$(MAKE) -C CustomShape

.PHONY: Deployment
Deployment:
	$(MAKE) -C Deployment

.PHONY: EffectAttributes
EffectAttributes:
	$(MAKE) -C EffectAttributes

.PHONY: clean
clean:
	$(MAKE) -C CannedForceEffect clean
	$(MAKE) -C CustomForceEffect clean
	$(MAKE) -C CustomShape clean
	$(MAKE) -C Deployment clean
	$(MAKE) -C EffectAttributes clean


