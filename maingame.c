// we need the include files io and stdlib and string and stdbool and time
// we need to define a constant for the number of rooms
// we need to define a constant for the maximum number of connections per room
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

// we want to write a text based game that is a little bit like a choose your own adventure book
// each turn we will present the player with a choice of actions
// the player will choose an action and we will carry it out
// the player can look at the room, or move to another room

// we will need to keep track of the current room
// we need a way to represent the rooms
// we need a way to represent the connections between the rooms
// let's have a datastructure that represents the rooms
// room has connections and connections have rooms so we need to define connection first but we can't because room needs it
// we can use a forward declaration to tell the compiler that connection is a struct that will be defined later
typedef struct connection connection;

typedef struct item {
  char* name;
  char* description;
} item;

typedef struct room {
  char* name;
  char* description;
  item* item;
  connection* connections;
  int numConnections;
} room;

// we need a way to represent the connections between the rooms
// let's have a datastructure that represents the connections
typedef struct connection {
  room* room1;
  room* room2;
} connection;

// we need a way to represent the player
// let's have a datastructure that represents the player
typedef struct player {
  room* currentRoom;
} player;

// we need a way to represent the game
// let's have a datastructure that represents the game
typedef struct game {
  room* rooms;
  int numRooms;
  player* player;
} game;

// Define an enum for menu choices
typedef enum {
    LOOK_AROUND = 1,
    MOVE_TO_ANOTHER_ROOM,
    QUIT
} MenuChoice;

// let's have a function to print a menu and get user choice, return the choice
int getMenuChoice() {
  int choice;
  printf("What would you like to do?\n");
  printf("1. Look around\n");
  printf("2. Move to another room\n");
  printf("3. Quit\n");
  scanf("%d", &choice);
  
  // we need to check that the choice is valid
  while (choice < LOOK_AROUND || choice > QUIT) {
    printf("Invalid choice, please try again\n");
    
    // Clear the input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    // Get new input
    scanf("%d", &choice);
  }

  return choice;
}

// Function to remove leading and trailing quotation marks from a string
void stripQuotationMarks(char* str) {
  int length = strlen(str);
  if (length > 0 && str[0] == '\"') {
    memmove(str, str + 1, length - 1);
    str[length - 1] = '\0';
  }
  if (length > 1 && str[length - 1] == '\"') {
    str[length - 1] = '\0';
  }
}

// let's have a function to print the room description
void printRoomDescription(room* room) {
  printf("You are in the %s.\n", room->name);
  printf("%s\n", room->description);
}

// let's have a function to print the room item
void printRoomItem(room* room) {
  printf("You look around the %s.\n", room->name);
  printf("You find %s.\n", room->item->name);
  printf("%s\n", room->item->description);
}

// a function to get user choice of room to move to
int getRoomChoice(room* currentRoom) {
  int choice;
  printf("Which room would you like to move to?\n");
  for (int i = 0; i < currentRoom->numConnections; i++) {
    printf("%d. %s\n", i+1, currentRoom->connections[i].room2->name);
  }
  scanf("%d", &choice);
  // we need to check that the choice is valid
  while (choice < 1 || choice > 3) {
    printf("Invalid choice, please try again\n");
    
    // Clear the input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    // Get new input
    scanf("%d", &choice);
  }
  return choice;
}

// a function to move the player to another room, and describe it to the user
void movePlayer(player* player, int choice) {
  player->currentRoom = player->currentRoom->connections[choice-1].room2;
  printRoomDescription(player->currentRoom);
}

void loadConnections(const char* filename, room* rooms, int numRooms) {
  FILE* file = fopen(filename, "r");

  if (file == NULL) {
   perror("Error opening connections file");
   exit(EXIT_FAILURE);
  }

  // Adjust the buffer size as needed
  char line[100];  

  for (int i = 0; i < numRooms; ++i) {
    if (fgets(line, sizeof(line), file) == NULL) {
      fprintf(stderr, "Error reading connections file\n");
      exit(EXIT_FAILURE);
    }

    // Assuming the connection data is comma-separated
    char* token = strtok(line, ",");
    while (token != NULL) {
      int roomIndex = atoi(token);

      // Create a connection between the current room and the connected room
      connection conn;
      conn.room1 = &rooms[i];
      conn.room2 = &rooms[roomIndex];

      // Update rooms with the connection
      rooms[i].connections = realloc(rooms[i].connections, sizeof(connection) * (rooms[i].numConnections + 1));
      rooms[i].connections[rooms[i].numConnections] = conn;
      rooms[i].numConnections++;

      token = strtok(NULL, ",");
    }
  }
  fclose(file);
}

// Function to load fantasy adventure items from a CSV file
item* loadItems() {
  FILE* file = fopen("items.csv", "r");
  if (file == NULL) {
    perror("Error opening items.csv");
    exit(EXIT_FAILURE);
  }

  int numItems = 30;
  item* items = malloc(sizeof(item) * numItems);

  char line[500];
  for (int i = 0; i < numItems; i++) {
    if (fgets(line, sizeof(line), file) == NULL) {
      perror("Error reading items.csv");
      exit(EXIT_FAILURE);
    }
    
    // Remove the newline character
    line[strlen(line) - 1] = '\0';  

    // Split the line using commas
    char* name = strtok(line, "\"");
    char* endofname=strtok(NULL, "\"");
    char* description = strtok(NULL, "\"");
    char* endofdesc=strtok(NULL, "\0");
   
    name[endofname-name]='\0';
    
    // Allocate memory for item
    items[i].name = malloc(sizeof(char) * strlen(name) + 1);
    items[i].description = malloc(sizeof(char) * strlen(description) + 1);

    // Copy values to item
    strcpy(items[i].name, name);
    strcpy(items[i].description, description);
  }

  fclose(file);
  return items;
}

// Function to randomly assign an item to each room
void assignItemsToRooms(room* rooms, int numRooms, item* items, int numItems) {
  srand(time(NULL)); // Seed for randomization

  for (int i = 0; i < numRooms; i++) {
    int randomIndex = rand() % numItems;
    rooms[i].item = &items[randomIndex];
  }
}

// a function to load the rooms from a file
// the file is called rooms.csv, and has a room name and room description on each line
// the function returns an array of rooms
room* loadRooms() {
  // open the file
  FILE* file = fopen("rooms.csv", "r");
  // we need to check that the file opened successfully
  if (file == NULL) {
    printf("Error opening file\n");
    exit(1);
  }
  
  // we need to count the number of lines in the file
  int numLines = 0;
  char* line = NULL;
  size_t len = 0;
  while (getline(&line, &len, file) != -1) {
    numLines++;
  }
  free(line);
  // we need to rewind the file
  rewind(file);
  // we need to allocate memory for the rooms
  room* rooms = malloc(sizeof(room) * numLines);
  // we need to read the rooms from the file
  for (int i = 0; i < numLines; i++) {
    // we need to read the line
    getline(&line, &len, file);
    // we need to remove the newline character
    line[strlen(line)-1] = '\0';
    // we need to split the line into the name and description
    // the strings are in quotation marks, and the two quoted strings are separated by a comma
    // we need to split the line on ", " (the four characters)
    //everything between the first and second " is the name
    //everything between the third and fourth " is the description
    // we need to find the first "
    char* name = strtok(line, "\"");
    // we need to find the second " and record where it is so we can null terminate the string
    char* endofname=strtok(NULL, "\"");
    
    // we need to find the third "
    char* description = strtok(NULL, "\"");
    // we need to find the fourth "
    char* endofdesc=strtok(NULL, "\0");
    //we need to be sure that name and description are null terminated strings
    name[endofname-name]='\0';
    //description[endofdesc-description]='\0';
    // we need to create a room
    room room;
    //we need to copy the string into the room name
    room.name = malloc(sizeof(char) * strlen(name) + 1);
    strcpy(room.name, name);
    //we need to copy the string into the room description
    room.description = malloc(sizeof(char) * strlen(description) + 1);
    strcpy(room.description, description);
    room.connections = NULL;
    room.numConnections = 0;
    // we need to add the room to the array
    rooms[i] = room;
  }
  free(line);
  // we need to close the file
  fclose(file);

  // Open the connections.csv connectionfile
  loadConnections("connections.csv", rooms, numLines);

  // Load fantasy adventure items
  int numItems = 30;
  item* items = loadItems(numItems);

  // Assign items to rooms
  assignItemsToRooms(rooms, numLines, items, numItems);

  // we need to return the array of rooms
  return rooms;
}

// let's have a function to create a player
player* createPlayer(room* currentRoom) {
  // we need to allocate memory for the player
  player* player = malloc(sizeof(player));
  // we need to set the current room
  player->currentRoom = currentRoom;
  // we need to return the player
  return player;
}

// let's have a function to create a game
game* createGame() {
  // we need to allocate memory for the game
  game* game = malloc(sizeof(game));
  if (game == NULL) {
    perror("Error creating game");
    exit(EXIT_FAILURE);
  }
  // we need to load the rooms
  printf("debug - about to load rooms\n");
  game->rooms = loadRooms();
  printf("debug - rooms loaded\n");
  // we need to create the player
  game->player = createPlayer(&game->rooms[0]);
  // we need to return the game
  return game;
}

// let's have a function to play the game which is the main function
void playGame() {
  // we need to create the game
  printf("Welcome to the game\n");
  game* game = createGame();
  // we need to print the room description
  printRoomDescription(game->player->currentRoom);
  // we need to loop until the user quits
  bool quit = false;
  while (!quit) {
    // we need to print the menu and get the user choice
    int choice = getMenuChoice();
    // we need to carry out the user choice
    switch (choice) {
      case LOOK_AROUND:
        // we need to print the room description
        printRoomItem(game->player->currentRoom);
        break;
      case MOVE_TO_ANOTHER_ROOM:
        // we need to get the user choice of room to move to
        choice = getRoomChoice(game->player->currentRoom);
        // we need to move the player to the room
        movePlayer(game->player, choice);
        break;
      case QUIT:
        // we need to quit
        quit = true;
        break;
      default:
        printf("Invalid choice, please try again\n");
        break; 
    }
  }
  free(game);
}

// let's have a main function to call the playGame function
int main() {
  playGame();
  return 0;
}

