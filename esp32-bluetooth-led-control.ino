#include "BluetoothSerial.h"

// Create a Bluetooth Serial object
BluetoothSerial SerialBT;

// Configuration and constants
#define VOICE_TIMEOUT 5000 // 5 seconds voice command timeout
// The LDR_INTERVAL definition and related LDR variables (ldrPin, lastLdrRead, ldrValue) have been removed.

// PWM configuration
const int freq = 5000;
const int resolution = 8; // 8-bit resolution (0-255)

// Room structure definition
struct Room {
  const char* name;
  // RGB LED pins
  int redPin;
  int greenPin;
  int bluePin;
  // PWM channels
  int redChannel;
  int greenChannel;
  int blueChannel;
  // Color values (last set color)
  int red;
  int green;
  int blue;
  // Brightness (0-255)
  int brightness;
  // Status (on/off)
  bool active;
};

// Room definitions
Room kitchen = {"Kitchen", 5, 4, 2, 0, 1, 2, 0, 0, 0, 255, false};
Room livingRoom = {"Living Room", 14, 12, 13, 3, 4, 5, 0, 0, 0, 255, false};
Room hall = {"Hall", 18, 19, 21, 6, 7, 8, 0, 0, 0, 255, false};

// Command processing
String receivedCommand = "";
boolean newCommand = false;

// For voice recognition mode
unsigned long lastVoiceTime = 0;
boolean voiceCommandMode = false;

// Color values (RGB format)
const int COLOR_RED[] = {255, 0, 0};
const int COLOR_GREEN[] = {0, 255, 0};
const int COLOR_BLUE[] = {0, 0, 255};
const int COLOR_YELLOW[] = {255, 255, 0};
const int COLOR_PURPLE[] = {255, 0, 255};
const int COLOR_CYAN[] = {0, 255, 255};
const int COLOR_WHITE[] = {255, 255, 255};
const int COLOR_OFF[] = {0, 0, 0};

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize Bluetooth
  SerialBT.begin("ESP32_Home_Control"); // Bluetooth name
  Serial.println("Bluetooth voice control active, device is discoverable");

  // Configure rooms (PWM setup)
  setupRoom(&kitchen);
  setupRoom(&livingRoom);
  setupRoom(&hall);

  // Set the initial state of all LEDs to OFF
  updateRoomLEDs(&kitchen, 0, 0, 0);
  updateRoomLEDs(&livingRoom, 0, 0, 0);
  updateRoomLEDs(&hall, 0, 0, 0);

  // Send initial message
  SerialBT.println("ESP32 Home Control System ready. Say or send 'Listen' command.");
  showHelp();
}

void loop() {
  // Check for incoming data from Bluetooth
  if (SerialBT.available()) {
    receivedCommand = SerialBT.readStringUntil('\n');
    receivedCommand.trim(); // Clean up spaces and newline characters
    newCommand = true;

    // Print the received command to the serial monitor
    Serial.print("Received command: ");
    Serial.println(receivedCommand);

    // Check for voice control mode activation
    if (receivedCommand.equalsIgnoreCase("dinle") || 
        receivedCommand.equalsIgnoreCase("listen")) {
      voiceCommandMode = true;
      lastVoiceTime = millis();
      SerialBT.println("Listening... (5 seconds)");
      blinkAllLEDs(); // Indicate entering listening mode by blinking LEDs
    }
  }

  // Check if voice command mode is active
  if (voiceCommandMode) {
    // Timeout check
    if (millis() - lastVoiceTime > VOICE_TIMEOUT) {
      voiceCommandMode = false;
      SerialBT.println("Voice listening timeout. Say 'Listen' to start again.");
    }

    // Process voice command if a new command (and not 'listen' command itself) is received
    if (newCommand && !receivedCommand.equalsIgnoreCase("dinle") && 
        !receivedCommand.equalsIgnoreCase("listen")) {
      processCommand(receivedCommand);
      lastVoiceTime = millis(); // Refresh timeout timer on command reception
      // Exit voice command mode after processing one command
      voiceCommandMode = false; 
    }
  } else {
    // Normal mode - process commands directly
    if (newCommand) {
      processCommand(receivedCommand);
    }
  }

  // LDR reading function has been removed from loop.

  newCommand = false;
}

// Function to configure the room (PWM setup)
void setupRoom(Room* room) {
  // Configure PWM channels
  ledcSetup(room->redChannel, freq, resolution);
  ledcSetup(room->greenChannel, freq, resolution);
  ledcSetup(room->blueChannel, freq, resolution);
  
  // Attach pins to channels
  ledcAttachPin(room->redPin, room->redChannel);
  ledcAttachPin(room->greenPin, room->greenChannel);
  ledcAttachPin(room->bluePin, room->blueChannel);
}

// Function to process commands
void processCommand(String command) {
  command.toLowerCase(); // Convert command to lowercase (for case-insensitivity)

  // Help command
  if (command.indexOf("yardım") >= 0 || command.indexOf("help") >= 0 ||
      command.indexOf("komut") >= 0 || command.indexOf("command") >= 0) {
    showHelp();
    return;
  }

  // LDR command removal: LDR reading and reporting command has been removed.

  // Brightness commands
  if (command.indexOf("azalt") >= 0 || command.indexOf("düşür") >= 0 || 
      command.indexOf("yükselt") >= 0 || command.indexOf("artır") >= 0 || 
      command.indexOf("parlaklık") >= 0 || command.indexOf("decrease") >= 0 ||
      command.indexOf("increase") >= 0 || command.indexOf("brightness") >= 0) {
    processBrightnessCommand(command);
    return;
  }

  // Room and color control
  String roomName = "";
  String colorName = "";

  // Determine the room
  if (command.indexOf("mutfak") >= 0 || command.indexOf("kitchen") >= 0) {
    roomName = "kitchen";
  } else if (command.indexOf("oturma") >= 0 || command.indexOf("oda") >= 0 || command.indexOf("living") >= 0) {
    roomName = "livingRoom";
  } else if (command.indexOf("salon") >= 0 || command.indexOf("hall") >= 0) {
    roomName = "hall";
  } else if (command.indexOf("hepsi") >= 0 || command.indexOf("tüm") >= 0 || command.indexOf("all") >= 0) {
    roomName = "all";
  }

  // Switch OFF command
  if (command.indexOf("kapat") >= 0 || command.indexOf("off") >= 0 || command.indexOf("söndür") >= 0 || command.indexOf("switch off") >= 0) {
    switchOff(roomName);
    return;
  }

  // Determine the color
  if (command.indexOf("kırmızı") >= 0 || command.indexOf("kirmizi") >= 0 || command.indexOf("red") >= 0) {
    colorName = "red";
  } else if (command.indexOf("yeşil") >= 0 || command.indexOf("yesil") >= 0 || command.indexOf("green") >= 0) {
    colorName = "green";
  } else if (command.indexOf("mavi") >= 0 || command.indexOf("blue") >= 0) {
    colorName = "blue";
  } else if (command.indexOf("sarı") >= 0 || command.indexOf("sari") >= 0 || command.indexOf("yellow") >= 0) {
    colorName = "yellow";
  } else if (command.indexOf("mor") >= 0 || command.indexOf("purple") >= 0) {
    colorName = "purple";
  } else if (command.indexOf("turkuaz") >= 0 || command.indexOf("cyan") >= 0) {
    colorName = "cyan";
  } else if (command.indexOf("beyaz") >= 0 || command.indexOf("white") >= 0) {
    colorName = "white";
  }

  // Apply room and color if both are specified
  if (roomName != "" && colorName != "") {
    setColor(roomName, colorName);
  } else if (roomName != "" && (command.indexOf("aç") >= 0 || command.indexOf("on") >= 0 || command.indexOf("yak") >= 0 || command.indexOf("switch on") >= 0)) {
    // If only room is specified and it's an ON command, switch on with the last set color
    switchOn(roomName);
  } else if (colorName != "") {
    // If only color is specified, apply the color to all active rooms
    applyColorToActiveLights(colorName);
  } else if (!command.equalsIgnoreCase("dinle") && !command.equalsIgnoreCase("listen")) {
    SerialBT.println("Command not understood: '" + command + "'. Type 'help' for commands.");
  }
}

// Function to process brightness commands
void processBrightnessCommand(String command) {
  String roomName = "";

  // Determine the room to apply the command
  if (command.indexOf("mutfak") >= 0 || command.indexOf("kitchen") >= 0) {
    roomName = "kitchen";
  } else if (command.indexOf("oturma") >= 0 || command.indexOf("oda") >= 0 || command.indexOf("living") >= 0) {
    roomName = "livingRoom";
  } else if (command.indexOf("salon") >= 0 || command.indexOf("hall") >= 0) {
    roomName = "hall";
  } else {
    roomName = "all"; // Apply to all if no room is specified
  }

  // Decrease brightness
  if (command.indexOf("azalt") >= 0 || command.indexOf("düşür") >= 0 || command.indexOf("decrease") >= 0) {
    adjustBrightness(roomName, -64); // Decrease by 64 (approx. 25%)
  }
  // Increase brightness
  else if (command.indexOf("yükselt") >= 0 || command.indexOf("artır") >= 0 || command.indexOf("increase") >= 0) {
    adjustBrightness(roomName, 64); // Increase by 64 (approx. 25%)
  }
}

// LDR reading and reporting function has been removed.

// Function to set the specified color to the specified room
void setColor(String roomName, String colorName) {
  int r = 0, g = 0, b = 0;

  // Determine color values
  if (colorName == "red") {
    r = COLOR_RED[0]; g = COLOR_RED[1]; b = COLOR_RED[2];
  } else if (colorName == "green") {
    r = COLOR_GREEN[0]; g = COLOR_GREEN[1]; b = COLOR_GREEN[2];
  } else if (colorName == "blue") {
    r = COLOR_BLUE[0]; g = COLOR_BLUE[1]; b = COLOR_BLUE[2];
  } else if (colorName == "yellow") {
    r = COLOR_YELLOW[0]; g = COLOR_YELLOW[1]; b = COLOR_YELLOW[2];
  } else if (colorName == "purple") {
    r = COLOR_PURPLE[0]; g = COLOR_PURPLE[1]; b = COLOR_PURPLE[2];
  } else if (colorName == "cyan") {
    r = COLOR_CYAN[0]; g = COLOR_CYAN[1]; b = COLOR_CYAN[2];
  } else if (colorName == "white") {
    r = COLOR_WHITE[0]; g = COLOR_WHITE[1]; b = COLOR_WHITE[2];
  } else {
    // Fallback for Turkish color names
    if (colorName == "kirmizi") {
      r = COLOR_RED[0]; g = COLOR_RED[1]; b = COLOR_RED[2];
    } else if (colorName == "yesil") {
      r = COLOR_GREEN[0]; g = COLOR_GREEN[1]; b = COLOR_GREEN[2];
    } else if (colorName == "mavi") {
      r = COLOR_BLUE[0]; g = COLOR_BLUE[1]; b = COLOR_BLUE[2];
    } else if (colorName == "sari") {
      r = COLOR_YELLOW[0]; g = COLOR_YELLOW[1]; b = COLOR_YELLOW[2];
    } else if (colorName == "mor") {
      r = COLOR_PURPLE[0]; g = COLOR_PURPLE[1]; b = COLOR_PURPLE[2];
    } else if (colorName == "turkuaz") {
      r = COLOR_CYAN[0]; g = COLOR_CYAN[1]; b = COLOR_CYAN[2];
    } else if (colorName == "beyaz") {
      r = COLOR_WHITE[0]; g = COLOR_WHITE[1]; b = COLOR_WHITE[2];
    }
  }

  // Apply to room(s)
  if (roomName == "kitchen") {
    updateRoomLEDs(&kitchen, r, g, b);
    kitchen.active = true;
    SerialBT.println("Kitchen set to " + colorName);
  } else if (roomName == "livingRoom") {
    updateRoomLEDs(&livingRoom, r, g, b);
    livingRoom.active = true;
    SerialBT.println("Living Room set to " + colorName);
  } else if (roomName == "hall") {
    updateRoomLEDs(&hall, r, g, b);
    hall.active = true;
    SerialBT.println("Hall set to " + colorName);
  } else if (roomName == "all") {
    updateRoomLEDs(&kitchen, r, g, b);
    updateRoomLEDs(&livingRoom, r, g, b);
    updateRoomLEDs(&hall, r, g, b);
    kitchen.active = true;
    livingRoom.active = true;
    hall.active = true;
    SerialBT.println("All rooms set to " + colorName);
  }
}

// Function to apply a color to all currently active lights
void applyColorToActiveLights(String colorName) {
  boolean anyActive = false;

  // Check if any room is active
  if (kitchen.active || livingRoom.active || hall.active) {
    anyActive = true;
  } else {
    // If no room is active, activate all lights to apply the color
    kitchen.active = true;
    livingRoom.active = true;
    hall.active = true;
    anyActive = true;
  }

  if (anyActive) {
    // Apply the color to active rooms
    if (kitchen.active) setColor("kitchen", colorName);
    if (livingRoom.active) setColor("livingRoom", colorName);
    if (hall.active) setColor("hall", colorName);
    SerialBT.println("Color " + colorName + " applied to active lights.");
  } else {
    SerialBT.println("No lights are active to apply the color.");
  }
}

// Function to switch off the specified room
void switchOff(String roomName) {
  if (roomName == "kitchen") {
    updateRoomLEDs(&kitchen, 0, 0, 0);
    kitchen.active = false;
    SerialBT.println("Kitchen lights switched off");
  } else if (roomName == "livingRoom") {
    updateRoomLEDs(&livingRoom, 0, 0, 0);
    livingRoom.active = false;
    SerialBT.println("Living Room lights switched off");
  } else if (roomName == "hall") {
    updateRoomLEDs(&hall, 0, 0, 0);
    hall.active = false;
    SerialBT.println("Hall lights switched off");
  } else if (roomName == "all") {
    updateRoomLEDs(&kitchen, 0, 0, 0);
    updateRoomLEDs(&livingRoom, 0, 0, 0);
    updateRoomLEDs(&hall, 0, 0, 0);
    kitchen.active = false;
    livingRoom.active = false;
    hall.active = false;
    SerialBT.println("All lights switched off");
  } else {
    SerialBT.println("Room to switch off not specified");
  }
}

// Function to switch on the specified room (with the last used color)
void switchOn(String roomName) {
  if (roomName == "kitchen") {
    updateRoomLEDs(&kitchen, kitchen.red, kitchen.green, kitchen.blue);
    kitchen.active = true;
    SerialBT.println("Kitchen lights switched on");
  } else if (roomName == "livingRoom") {
    updateRoomLEDs(&livingRoom, livingRoom.red, livingRoom.green, livingRoom.blue);
    livingRoom.active = true;
    SerialBT.println("Living Room lights switched on");
  } else if (roomName == "hall") {
    updateRoomLEDs(&hall, hall.red, hall.green, hall.blue);
    hall.active = true;
    SerialBT.println("Hall lights switched on");
  } else if (roomName == "all") {
    updateRoomLEDs(&kitchen, kitchen.red, kitchen.green, kitchen.blue);
    updateRoomLEDs(&livingRoom, livingRoom.red, livingRoom.green, livingRoom.blue);
    updateRoomLEDs(&hall, hall.red, hall.green, hall.blue);
    kitchen.active = true;
    livingRoom.active = true;
    hall.active = true;
    SerialBT.println("All lights switched on");
  } else {
    SerialBT.println("Room to switch on not specified");
  }
}

// Function for brightness adjustment
void adjustBrightness(String roomName, int amount) {
  // Update kitchen brightness
  if (roomName == "kitchen" || roomName == "all") {
    kitchen.brightness = constrain(kitchen.brightness + amount, 0, 255);
    updateRoomLEDs(&kitchen, kitchen.red, kitchen.green, kitchen.blue);
    SerialBT.print("Kitchen brightness: %");
    SerialBT.println(map(kitchen.brightness, 0, 255, 0, 100));
  }

  // Update living room brightness
  if (roomName == "livingRoom" || roomName == "all") {
    livingRoom.brightness = constrain(livingRoom.brightness + amount, 0, 255);
    updateRoomLEDs(&livingRoom, livingRoom.red, livingRoom.green, livingRoom.blue);
    SerialBT.print("Living Room brightness: %");
    SerialBT.println(map(livingRoom.brightness, 0, 255, 0, 100));
  }

  // Update hall brightness
  if (roomName == "hall" || roomName == "all") {
    hall.brightness = constrain(hall.brightness + amount, 0, 255);
    updateRoomLEDs(&hall, hall.red, hall.green, hall.blue);
    SerialBT.print("Hall brightness: %");
    SerialBT.println(map(hall.brightness, 0, 255, 0, 100));
  }
}

// Function to update room LEDs with PWM values
void updateRoomLEDs(Room* room, int r, int g, int b) {
  // Save the RGB values (for 'switchOn' and color application when active)
  room->red = r;
  room->green = g;
  room->blue = b;

  // Apply the brightness factor to the RGB values
  int adjustedR = map(r, 0, 255, 0, room->brightness);
  int adjustedG = map(g, 0, 255, 0, room->brightness);
  int adjustedB = map(b, 0, 255, 0, room->brightness);

  // Update LEDs using PWM
  ledcWrite(room->redChannel, adjustedR);
  ledcWrite(room->greenChannel, adjustedG);
  ledcWrite(room->blueChannel, adjustedB);
}

// Function to display help information
void showHelp() {
  SerialBT.println("\n--- ESP32 Home Control Commands ---");
  SerialBT.println("1. 'listen' or 'dinle' - Activates voice listening mode (5 sec timeout)");
  SerialBT.println("2. Room commands: 'kitchen', 'living room', 'hall', 'all' (or Turkish counterparts)");
  SerialBT.println("3. Color commands: 'red', 'green', 'blue', 'yellow', 'purple', 'cyan', 'white'");
  SerialBT.println("4. Example: 'kitchen red' - Sets kitchen lights to red");
  SerialBT.println("5. On/Off: 'hall switch on', 'all switch off', 'kitchen kapat'");
  SerialBT.println("6. Brightness: 'kitchen brightness decrease', 'hall brightness increase' (or 'azalt'/'artır')");
  SerialBT.println("7. 'help' or 'yardım' - Shows this help message");
}

// Function to blink all LEDs to indicate listening mode
void blinkAllLEDs() {
  // Save current states
  int savedMutfakR = kitchen.red;
  int savedMutfakG = kitchen.green;
  int savedMutfakB = kitchen.blue;

  int savedOturmaR = livingRoom.red;
  int savedOturmaG = livingRoom.green;
  int savedOturmaB = livingRoom.blue;

  int savedSalonR = hall.red;
  int savedSalonG = hall.green;
  int savedSalonB = hall.blue;

  // Blinking effect
  for(int i=0; i<3; i++) {
    // Set all to white
    updateRoomLEDs(&kitchen, 255, 255, 255);
    updateRoomLEDs(&livingRoom, 255, 255, 255);
    updateRoomLEDs(&hall, 255, 255, 255);
    delay(150);

    // Turn all off
    updateRoomLEDs(&kitchen, 0, 0, 0);
    updateRoomLEDs(&livingRoom, 0, 0, 0);
    updateRoomLEDs(&hall, 0, 0, 0);
    delay(150);
  }

  // Restore previous state
  updateRoomLEDs(&kitchen, savedMutfakR, savedMutfakG, savedMutfakB);
  updateRoomLEDs(&livingRoom, savedOturmaR, savedOturmaG, savedOturmaB);
  updateRoomLEDs(&hall, savedSalonR, savedSalonG, savedSalonB);
}