#include "controller.h"
#include "joystick.h"
#include "global.h"
#include <SDL.h>
#include <map>
#include <string>
#include <sstream>


std::map<SDL_GameControllerAxis, std::string> controller::axes;
std::map<SDL_GameControllerButton, std::string> controller::buttons;


void
controller::getState (Napi::Env &env, SDL_GameController *controller, Napi::Object dst)
{
	const char *error = SDL_GetError();
	if (error != global::no_error) { fprintf(stderr, "SDL silent error: %s\n", error); }
	SDL_ClearError();

	Napi::Object axes = Napi::Object::New(env);
	axes.Set("leftStickX", Napi::Number::New(env, joystick::mapAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX))));
	axes.Set("leftStickY", Napi::Number::New(env, joystick::mapAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY))));
	axes.Set("rightStickX", Napi::Number::New(env, joystick::mapAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX))));
	axes.Set("rightStickY", Napi::Number::New(env, joystick::mapAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY))));
	axes.Set("leftTrigger", Napi::Number::New(env, joystick::mapAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT))));
	axes.Set("rightTrigger", Napi::Number::New(env, joystick::mapAxis(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT))));

	error = SDL_GetError();
	if (error != global::no_error) {
		std::ostringstream message;
		message << "SDL_GameControllerGetAxis() error: " << error;
		SDL_ClearError();
		throw Napi::Error::New(env, message.str());
	}

	Napi::Object buttons = Napi::Object::New(env);
	buttons.Set("dpadLeft", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT)));
	buttons.Set("dpadRight", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)));
	buttons.Set("dpadUp", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP)));
	buttons.Set("dpadDown", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN)));
	buttons.Set("a", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A)));
	buttons.Set("b", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B)));
	buttons.Set("x", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X)));
	buttons.Set("y", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y)));
	buttons.Set("guide", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_GUIDE)));
	buttons.Set("back", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_BACK)));
	buttons.Set("start", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START)));
	buttons.Set("leftStick", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_LEFTSTICK)));
	buttons.Set("rightStick", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_RIGHTSTICK)));
	buttons.Set("leftShoulder", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER)));
	buttons.Set("rightShoulder", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)));
	buttons.Set("paddle1", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_PADDLE1)));
	buttons.Set("paddle2", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_PADDLE2)));
	buttons.Set("paddle3", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_PADDLE3)));
	buttons.Set("paddle4", Napi::Boolean::New(env, SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_PADDLE4)));

	error = SDL_GetError();
	if (error != global::no_error) {
		std::ostringstream message;
		message << "SDL_GameControllerGetButton() error: " << error;
		SDL_ClearError();
		throw Napi::Error::New(env, message.str());
	}

	dst.Set("axes", axes);
	dst.Set("buttons", buttons);
}


Napi::Value
controller::addMappings (const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();

	Napi::Array mappings = info[0].As<Napi::Array>();

	for (int i = 0; i < (int) mappings.Length(); i++) {
		std::string mapping = mappings.Get(i).As<Napi::String>().Utf8Value();
		if (SDL_GameControllerAddMapping(mapping.c_str()) == -1) {
			std::ostringstream message;
			message << "SDL_GameControllerAddMapping(" << mapping << ") error: " << SDL_GetError();
			SDL_ClearError();
			throw Napi::Error::New(env, message.str());
		}
	}
	return env.Undefined();
}

Napi::Value
controller::open (const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();

	int index = info[0].As<Napi::Number>().Int32Value();

	SDL_GameController *controller = SDL_GameControllerOpen(index);
	if (controller == nullptr) {
		std::ostringstream message;
		message << "SDL_GameControllerOpen(" << index << ") error: " << SDL_GetError();
		SDL_ClearError();
		throw Napi::Error::New(env, message.str());
	}

	SDL_Joystick *joystick = SDL_GameControllerGetJoystick(controller);
	if (joystick == nullptr) {
		std::ostringstream message;
		message << "SDL_GameControllerGetJoystick(" << index << ") error: " << SDL_GetError();
		SDL_ClearError();
		throw Napi::Error::New(env, message.str());
	}

	SDL_JoystickID joystick_id = SDL_JoystickInstanceID(joystick);
	if (joystick_id < 0) {
		std::ostringstream message;
		message << "SDL_JoystickInstanceID(" << index << ") error: " << SDL_GetError();
		SDL_ClearError();
		throw Napi::Error::New(env, message.str());
	}

	const char *controller_name = SDL_GameControllerName(controller);
	Napi::Value controller_name_node = controller_name == 0 ? env.Null() : Napi::String::New(env, controller_name);

	int firmware = SDL_GameControllerGetFirmwareVersion(controller);
	Napi::Value firmware_version = firmware == 0 ? env.Null() : Napi::Number::New(env, firmware);

	int vendor = SDL_GameControllerGetVendor(controller);
	Napi::Value vendor_node = vendor == 0 ? env.Null() : Napi::Number::New(env, vendor);

	int product = SDL_GameControllerGetProduct(controller);
	Napi::Value product_node = product == 0 ? env.Null() : Napi::Number::New(env, product);

	const char *serial = SDL_GameControllerGetSerial(controller);
	Napi::Value serial_number = serial == 0 ? env.Null() : Napi::String::New(env, serial);

	bool has_led = SDL_GameControllerHasLED(controller);
	bool has_rumble = SDL_GameControllerHasRumble(controller);
	bool has_rumble_triggers = SDL_GameControllerHasRumbleTriggers(controller);

	int steam_handle = SDL_GameControllerGetSteamHandle(controller);
	api::Value steam_handle_node = steam_handle == 0 ? env.Null() : Napi::Number::New(env, steam_handle);
	
	const char *controller_path = SDL_GameControllerPath(controller);
	Napi::Value controller_path_node = controller_path == 0 ? env.Null() : Napi::String::New(env, controller_path);

	Napi::Object result = Napi::Object::New(env);
	// result.Set("id", Napi::Number::New(env, joystick_id)); // NOTE: Unused. Same as the device id
	result.Set("controllerName", controller_name_node);
	result.Set("firmwareVersion", firmware_version);
	result.Set("controllerVendor", vendor_node);
	result.Set("controllerProduct", product_node);
	result.Set("controllerPath", controller_path_node);
	result.Set("serialNumber", serial_number);
	result.Set("hasLed", Napi::Boolean::New(env, has_led));
	result.Set("hasRumble", Napi::Boolean::New(env, has_rumble));
	result.Set("hasRumbleTriggers", Napi::Boolean::New(env, has_rumble_triggers));
	result.Set("steam_handle", steam_handle_node);

	controller::getState(env, controller, result);

	return result;
}

Napi::Value
controller::close (const Napi::CallbackInfo &info)
{
	Napi::Env env = info.Env();

	int controller_id = info[0].As<Napi::Number>().Int32Value();

	SDL_GameController *controller = SDL_GameControllerFromInstanceID(controller_id);
	if (controller == nullptr) {
		std::ostringstream message;
		message << "SDL_GameControllerFromInstanceID(" << controller_id << ") error: " << SDL_GetError();
		SDL_ClearError();
		throw Napi::Error::New(env, message.str());
	}

	SDL_GameControllerClose(controller);

	return env.Undefined();
}
