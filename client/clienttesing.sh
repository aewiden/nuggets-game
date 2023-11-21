#!/bin/bash
# Group 13 - Lost at C
# CS50 Winter 2023
# clienttesting.sh
# Testing client.c to see if the file is working as intended
#
# NOTE: To set up for main.txt and challenge.txt servers, run them on two terminals and manually input the port number
#

rm -f client-output/clienttesting.log
# Testing no arguments
./client 
if [ $? -ne 0 ]
then
  echo "Invalid no arguments test passed!"
fi

# Testing too many arguments
./client 2>player.log plank 49242 phuc catinthehat lorax
if [ $? -ne 0 ]
then
  echo "Invalid too many arguments test passed!"
fi

# Testing bad hostname
./client 2>player.log globalhost 49242
if [ $? -ne 0 ]
then
  echo "Bad hostname test passed!"
fi

# Testing bad port number
./client 2>player.log plank 42
if [ $? -ne 0 ]
then
  echo "Bad port number test passed!"
fi

# Testing player with main.txt
# TO DO: it doesn't log stderr to player1.log
./client 2>player1.log plank 49242 phuc << EOF
kjhlnq
EOF

if [ $? -eq 0 ]
then
  echo "Normal client with main.txt test passed!"
fi

# Testing player with challenge.txt
./client 2>player2.log plank 48830 phuc << EOF
kjhlnq
EOF

if [ $? -eq 0 ]
then
  echo "Normal client test with challenge.txt passed!"
fi

# Testing spectator with challenge.txt
./client 2>spectator1.log plank 48830 << EOF
q
EOF

# Valgrind test with main.txt
valgrind --leak-check=full --show-leak-kinds=all ./client 2>player3mem.log  plank 49242 player3 << EOF
q
EOF

# Valgrind test with challenge.txt
valgrind --leak-check=full --show-leak-kinds=all ./client 2>player4mem.log  plank 48830 player4 << EOF
q
EOF