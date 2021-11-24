/**
 * @file	engine.h
 *
 * @date May 21, 2014
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#pragma once

#include "globalaccess.h"
#include "engine_module.h"
#include "engine_state.h"
#include "rpm_calculator.h"
#include "event_registry.h"
#include "table_helper.h"
#include "listener_array.h"
#include "accel_enrichment.h"
#include "trigger_central.h"
#include "local_version_holder.h"
#include "buttonshift.h"
#include "gear_controller.h"
#include "high_pressure_fuel_pump.h"
#include "limp_manager.h"
#include "pin_repository.h"
#include "ac_control.h"
#include "knock_logic.h"
#include "idle_state_generated.h"
#include "idle_thread.h"
#include "injector_model.h"
#include "launch_control.h"
#include "trigger_scheduler.h"
#include "fuel_pump.h"
#include "main_relay.h"
#include "type_list.h"

#ifndef EFI_UNIT_TEST
#error EFI_UNIT_TEST must be defined!
#endif

#ifndef EFI_SIMULATOR
#error EFI_SIMULATOR must be defined!
#endif

#ifndef EFI_PROD_CODE
#error EFI_PROD_CODE must be defined!
#endif

#if EFI_SIGNAL_EXECUTOR_ONE_TIMER
// PROD real firmware uses this implementation
#include "single_timer_executor.h"
#endif /* EFI_SIGNAL_EXECUTOR_ONE_TIMER */
#if EFI_SIGNAL_EXECUTOR_SLEEP
#include "signal_executor_sleep.h"
#endif /* EFI_SIGNAL_EXECUTOR_SLEEP */
#if EFI_UNIT_TEST
#include "global_execution_queue.h"
#endif /* EFI_UNIT_TEST */

#define FAST_CALLBACK_PERIOD_MS 5
#define SLOW_CALLBACK_PERIOD_MS 50

class RpmCalculator;
struct AirmassModelBase;

#define MAF_DECODING_CACHE_SIZE 256

#define MAF_DECODING_CACHE_MULT (MAF_DECODING_CACHE_SIZE / 5.0)

/**
 * I am not sure if this needs to be configurable.
 *
 * Also technically the whole feature might be implemented as cranking fuel coefficient curve by TPS.
 */
// todo: not great location for these
#define CLEANUP_MODE_TPS 90
#define STEPPER_PARKING_TPS CLEANUP_MODE_TPS

#define CYCLE_ALTERNATION 2

class IEtbController;
struct IFuelComputer;
struct IIdleController;

class PrimaryTriggerConfiguration final : public TriggerConfiguration {
public:
	PrimaryTriggerConfiguration() : TriggerConfiguration("TRG ") {}

protected:
	bool isUseOnlyRisingEdgeForTrigger() const override;
	bool isVerboseTriggerSynchDetails() const override;
	trigger_type_e getType() const override;
};

class VvtTriggerConfiguration final : public TriggerConfiguration {
public:
	const int index;

	VvtTriggerConfiguration(const char * prefix, const int index) : TriggerConfiguration(prefix), index(index) {
	}

protected:
	bool isUseOnlyRisingEdgeForTrigger() const override;
	bool isVerboseTriggerSynchDetails() const override;
	trigger_type_e getType() const override;
};

#define DEFAULT_MOCK_SPEED -1

class Engine final : public TriggerStateListener {
public:
	Engine();
	AcState acState;
	// todo: technical debt: enableOverdwellProtection #3553
	bool enableOverdwellProtection = true;

	// used by HW CI
	bool isPwmEnabled = true;

	const char *prevOutputName = nullptr;

	PinRepository pinRepository;

	InjectionEvent primeInjEvent;

	IEtbController *etbControllers[ETB_COUNT] = {nullptr};
	IFuelComputer *fuelComputer = nullptr;

	type_list<
		Mockable<InjectorModel>,
#if EFI_IDLE_CONTROL
		IdleController,
#endif // EFI_IDLE_CONTROL
		TriggerScheduler,
#if EFI_HPFP && EFI_ENGINE_CONTROL
		HpfpController,
#endif // EFI_HPFP && EFI_ENGINE_CONTROL

		FuelPumpController,
		MainRelayController,
		EngineModule // dummy placeholder so the previous entries can all have commas
		> engineModules;

	/**
	 * Slightly shorter helper function to keep the code looking clean.
	 */
	template<typename get_t>
	auto & module() {
		return engineModules.get<get_t>();
	}

	cyclic_buffer<int> triggerErrorDetection;

	GearControllerBase *gearController;
	LaunchControlBase launchController;
	SoftSparkLimiter softSparkLimiter;

	efitick_t mostRecentSparkEvent;
	efitick_t mostRecentTimeBetweenSparkEvents;
	efitick_t mostRecentIgnitionEvent;
	efitick_t mostRecentTimeBetweenIgnitionEvents;

	PrimaryTriggerConfiguration primaryTriggerConfiguration;
#if CAMS_PER_BANK == 1
	VvtTriggerConfiguration vvtTriggerConfiguration[CAMS_PER_BANK] = {{"VVT1 ", 0}};
#else
	VvtTriggerConfiguration vvtTriggerConfiguration[CAMS_PER_BANK] = {{"VVT1 ", 0}, {"VVT2 ", 1}};
#endif

	efitick_t startStopStateLastPushTime = 0;

#if EFI_SHAFT_POSITION_INPUT
	void OnTriggerStateDecodingError();
	void OnTriggerStateProperState(efitick_t nowNt) override;
	void OnTriggerSyncronization(bool wasSynchronized) override;
	void OnTriggerInvalidIndex(int currentIndex) override;
	void OnTriggerSynchronizationLost() override;
#endif

	void setConfig();
	injection_mode_e getCurrentInjectionMode();

	LocalVersionHolder versionForConfigurationListeners;
	LocalVersionHolder auxParametersVersion;
	operation_mode_e getOperationMode();

	AuxActor auxValves[AUX_DIGITAL_VALVE_COUNT][2];

#if EFI_UNIT_TEST
	bool needTdcCallback = true;
#endif /* EFI_UNIT_TEST */

	/**
	 * By the way 32-bit value should hold at least 400 hours of events at 6K RPM x 12 events per revolution
	 */
	int globalSparkIdCounter = 0;

	// this is useful at least for real hardware integration testing - maybe a proper solution would be to simply
	// GND input pins instead of leaving them floating
	bool hwTriggerInputEnabled = true;

	int getGlobalConfigurationVersion(void) const;


	// a pointer with interface type would make this code nicer but would carry extra runtime
	// cost to resolve pointer, we use instances as a micro optimization
#if EFI_SIGNAL_EXECUTOR_ONE_TIMER
	SingleTimerExecutor executor;
#endif
#if EFI_SIGNAL_EXECUTOR_SLEEP
	SleepExecutor executor;
#endif
#if EFI_UNIT_TEST
	TestExecutor executor;
#endif

#if EFI_ENGINE_CONTROL
	FuelSchedule injectionEvents;
	IgnitionEventList ignitionEvents;
	scheduling_s tdcScheduler[2];
#endif /* EFI_ENGINE_CONTROL */

	bool needToStopEngine(efitick_t nowNt) const;
	bool etbAutoTune = false;
	/**
	 * this is based on isEngineChartEnabled and engineSnifferRpmThreshold settings
	 */
	bool isEngineChartEnabled = false;

	bool tdcMarkEnabled = true; // used by unit tests only

	/**
	 * this is based on sensorChartMode and sensorSnifferRpmThreshold settings
	 */
	sensor_chart_e sensorChartMode = SC_OFF;

	bool slowCallBackWasInvoked = false;

	/**
	 * remote telemetry: if not zero, time to stop flashing 'CALL FROM PIT STOP' light
	 * todo: looks like there is a bug here? 64 bit storage an 32 bit time logic? anyway this feature is mostly a dream at this point
	 */
	efitimems64_t callFromPitStopEndTime = 0;

	RpmCalculator rpmCalculator;

	/**
	 * this is about 'stopengine' command
	 */
	efitick_t stopEngineRequestTimeNt = 0;


	bool startStopState = false;
	int startStopStateToggleCounter = 0;

	/**
	 * this is needed by getTimeIgnitionSeconds() and checkShutdown()
	 */
	efitick_t ignitionOnTimeNt = 0;

	/**
	 * This counter is incremented every time user adjusts ECU parameters online (either via rusEfi console or other
	 * tuning software)
	 */
	volatile int globalConfigurationVersion = 0;

	/**
	 * always 360 or 720, never zero
	 */
	angle_t engineCycle;

	TpsAccelEnrichment tpsAccelEnrichment;

	TriggerCentral triggerCentral;

	/**
	 * Each individual fuel injection duration for current engine cycle, without wall wetting
	 * including everything including injector lag, both cranking and running
	 * @see getInjectionDuration()
	 */
	floatms_t injectionDuration = 0;

	// Per-injection fuel mass, including TPS accel enrich
	float injectionMass[STFT_BANK_COUNT] = {0};

	float stftCorrection[STFT_BANK_COUNT] = {0};

	/**
	 * This one with wall wetting accounted for, used for logging.
	 */
	floatms_t actualLastInjection[STFT_BANK_COUNT] = {0};

	// Standard cylinder air charge - 100% VE at standard temperature, grams per cylinder
	float standardAirCharge = 0;

	void periodicFastCallback();
	void periodicSlowCallback();
	void updateSlowSensors();
	void updateSwitchInputs();
	void initializeTriggerWaveform();

	bool clutchUpState = false;
	bool clutchDownState = false;
	bool brakePedalState = false;

	// todo: extract some helper which would contain boolean state and most recent toggle time?
	bool acSwitchState = false;
	efitimeus_t acSwitchLastChangeTime = 0;

	bool isRunningPwmTest = false;

	FsioState fsioState;

	/**
	 * are we running any kind of functional test? this affect
	 * some areas
	 */
	bool isFunctionalTestMode = false;

	/**
	 * See also triggerSimulatorFrequency
	 */
	bool directSelfStimulation = false;

	void resetEngineSnifferIfInTestMode();

	/**
	 * pre-calculated offset for given sequence index within engine cycle
	 * (not cylinder ID)
	 */
	angle_t ignitionPositionWithinEngineCycle[MAX_CYLINDER_COUNT];
	/**
	 * pre-calculated reference to which output pin should be used for
	 * given sequence index within engine cycle
	 * todo: update documentation
	 */
	int ignitionPin[MAX_CYLINDER_COUNT];

	/**
	 * this is invoked each time we register a trigger tooth signal
	 */
	void onTriggerSignalEvent();
	EngineState engineState;
	idle_state_s idle;
	/**
	 * idle blip is a development tool: alternator PID research for instance have benefited from a repetitive change of RPM
	 */
	percent_t blipIdlePosition;
	efitimeus_t timeToStopBlip = 0;
	efitimeus_t timeToStopIdleTest = 0;


	SensorsState sensors;
	efitick_t mainRelayBenchStartNt = 0;

	/**
	 * This field is true if we are in 'cylinder cleanup' state right now
	 * see isCylinderCleanupEnabled
	 */
	bool isCylinderCleanupMode = false;

	/**
	 * value of 'triggerShape.getLength()'
	 * pre-calculating this value is a performance optimization
	 */
	uint32_t engineCycleEventCount = 0;

	void preCalculate();

	void watchdog();

	/**
	 * Needed by EFI_MAIN_RELAY_CONTROL to shut down the engine correctly.
	 * This method cancels shutdown if the ignition voltage is detected.
	 */
	void checkShutdown();

	/**
	 * Allows to finish some long-term shutdown procedures (stepper motor parking etc.)
	   Called when the ignition switch is turned off (vBatt is too low).
	   Returns true if some operations are in progress on background.
	 */
	bool isInShutdownMode() const;

	bool isInMainRelayBench();

	/**
	 * The stepper does not work if the main relay is turned off (it requires +12V).
	 * Needed by the stepper motor code to detect if it works.
	 */
	bool isMainRelayEnabled() const;

	/**
	 * Needed by EFI_MAIN_RELAY_CONTROL to handle fuel pump and shutdown timings correctly.
	 * This method returns the number of seconds since the ignition voltage is present.
	 * The return value is float for more FSIO flexibility.
	 */
	float getTimeIgnitionSeconds(void) const;

	void onSparkFireKnockSense(uint8_t cylinderIndex, efitick_t nowNt);

	KnockController knockController;

	AirmassModelBase* mockAirmassModel = nullptr;

	LimpManager limpManager;

private:
	/**
	 * By the way:
	 * 'cranking' means engine is not stopped and the rpm are below crankingRpm
	 * 'running' means RPM are above crankingRpm
	 * 'spinning' means the engine is not stopped
	 */
	bool isSpinning = false;
	void reset();

	void injectEngineReferences();
};

void prepareShapes();
void applyNonPersistentConfiguration();
void prepareOutputSignals();

void validateConfiguration();
void doScheduleStopEngine();

#define HW_CHECK_RPM 200

// These externs aren't needed for unit tests - everything is injected instead
#if !EFI_UNIT_TEST
extern Engine ___engine;
static Engine * const engine = &___engine;
#else // EFI_UNIT_TEST
extern Engine *engine;
#endif // EFI_UNIT_TEST
