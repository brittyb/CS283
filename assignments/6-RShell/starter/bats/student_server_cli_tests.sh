setup() {
    ./dsh -s &
    SERVER_PID=$!
    sleep 3
}


@test "connect server" {
        run ./dsh -c <<EOF
stop-server
EOF
        stripped_output=$(echo "$output" | tr -d '[:space:]')
        expected_output="socketclientmode:addr:127.0.0.1:1234dsh4>serverappearedtoterminate-exitingcmdloopreturned0"
        echo "Captured stdout:"
        echo "Output: $output"
        echo "Exit Status: $status"
        echo "${stripped_output} -> ${expected_output}"


        [ "$stripped_output" = "$expected_output" ]
        [ "$status" -eq 0 ]
}

@test "connect and execute non-bic" {
	run ./dsh -c <<EOF
echo testing
stop-server
EOF
        stripped_output=$(echo "$output" | tr -d '[:space:]')
        expected_output="socketclientmode:addr:127.0.0.1:1234dsh4>testingdsh4>serverappearedtoterminate-exitingcmdloopreturned0"
        echo "Captured stdout:"
        echo "Output: $output"
        echo "Exit Status: $status"
        echo "${stripped_output} -> ${expected_output}"


        [ "$stripped_output" = "$expected_output" ]
        [ "$status" -eq 0 ]
}

@test "connect and execute basic pipe command" {
	run ./dsh -c <<EOF
echo "hello world" | tr 'a-z' 'A-Z'
stop-server
EOF
        stripped_output=$(echo "$output" | tr -d '[:space:]')
        expected_output='socketclientmode:addr:127.0.0.1:1234dsh4>"HELLOWORLD"dsh4>serverappearedtoterminate-exitingcmdloopreturned0'
        echo "Captured stdout:"
        echo "Output: $output"
        echo "Exit Status: $status"
        echo "${stripped_output} -> ${expected_output}"


        [ "$stripped_output" = "$expected_output" ]
        [ "$status" -eq 0 ]
}

@test "execute command that does not exist" {
	run ./dsh -c <<EOF
fake_command
stop-server
EOF
	stripped_output=$(echo "$output" | sed 's/\n$//')
	expected_output="socketclientmode:addr:127.0.0.1:1234dsh4>socketservermode:addr:0.0.0.0:1234->Single-ThreadedModerdsh-exec:fake_commandCommandfaileddsh4>serverappearedtoterminate-exitingcmdloopreturned0"
        stripped_output=$(echo "$stripped_output" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')
	expected_output=$(echo "$expected_output" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')
	stripped_output=$(echo "$stripped_output" | tr -d '[:space:]')
	expected_output=$(echo "$expected_output" | tr -d '[:space:]')
	stripped_output=$(echo "$stripped_output" | tr -cd '\11\12\15\40-\176')
	echo "Length of stripped_output: ${#stripped_output}"
	echo "Length of expected_output: ${#expected_output}"
	echo "Captured stdout:"
        echo "Output: $stripped_output"
        echo "Exit Status: $status"
        echo "${stripped_output} -> ${expected_output}"	

        [ "$stripped_output" = "$expected_output" ]
        [ "$status" -eq 0 ]
}

@test "no command" {
	run ./dsh -c <<EOF

stop-server
EOF
	stripped_output=$(echo "$output" | tr -d '[:space:]')
	expected_output="socketclientmode:addr:127.0.0.1:1234dsh4>rdsh-error:commandexecutionerrordsh4>serverappearedtoterminate-exitingcmdloopreturned0"
	echo "Captured stdout:"
        echo "Output: $output"
        echo "Exit Status: $status"
        echo "${stripped_output} -> ${expected_output}"


        [ "$stripped_output" = "$expected_output" ]
        [ "$status" -eq 0 ]
}

@test "only spaces" {
	run ./dsh -c <<EOF
   
stop-server
EOF
	stripped_output=$(echo "$output" | tr -d '[:space:]')
        expected_output="socketclientmode:addr:127.0.0.1:1234dsh4>rdsh-error:commandexecutionerrordsh4>serverappearedtoterminate-exitingcmdloopreturned0"
        echo "Captured stdout:"
        echo "Output: $output"
        echo "Exit Status: $status"
        echo "${stripped_output} -> ${expected_output}"


        [ "$stripped_output" = "$expected_output" ]
        [ "$status" -eq 0 ]
}

@test "too many pipes" {
	run ./dsh -c <<EOF
echo 1 | echo 2 | echo 3 | echo 4 | echo 5 | echo 6 | echo 7 | echo 8 | echo 9 | echo 10
stop-server
EOF
	stripped_output=$(echo "$output" | tr -d '[:space:]')
        expected_output="socketclientmode:addr:127.0.0.1:1234dsh4>errorbuildingcmdlistdsh4>serverappearedtoterminate-exitingcmdloopreturned0"
        echo "Captured stdout:"
        echo "Output: $output"
        echo "Exit Status: $status"
        echo "${stripped_output} -> ${expected_output}"


        [ "$stripped_output" = "$expected_output" ]
        [ "$status" -eq 0 ]
}
