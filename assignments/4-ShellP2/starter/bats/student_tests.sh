#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "echo with and without quotes print the same output" {
    run ./dsh <<EOF
echo "hello"
echo hello                
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="hellohellodsh2>dsh2>dsh2>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "no command" {
    run ./dsh <<EOF

EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="dsh2>warning:nocommandsprovideddsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

 
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "command that does not exist" {
	run ./dsh <<EOF
fakecommand
rc
EOF
	stripped_output=$(echo "$output" | tr -d '[:space:]')
	expected_output="dsh2>CommandnotfoundinPATHdsh2>dsh2>2dsh2>cmdloopreturned0"
	echo "Captured stdout:"
    	echo "Output: $output"
    	echo "Exit Status: $status"
    	echo "${stripped_output} -> ${expected_output}"


    	[ "$stripped_output" = "$expected_output" ]
    	[ "$status" -eq 0 ]
}

@test "built in command with too many args" {
	run ./dsh <<EOF
cd a a a a a a a a a
rc
EOF
	stripped_output=$(echo "$output" | tr -d '[:space:]')
	expected_output="dsh2>dsh2>-4dsh2>cmdloopreturned0"
	echo "Captured stdout:"
        echo "Output: $output"
        echo "Exit Status: $status"
        echo "${stripped_output} -> ${expected_output}"


        [ "$stripped_output" = "$expected_output" ]
        [ "$status" -eq 0 ]
}

@test "non built in command with too many args" {
        run ./dsh <<EOF
echo a a a a a a a a a
rc
EOF
	stripped_output=$(echo "$output" | tr -d '[:space:]')
        expected_output="dsh2>dsh2>-4dsh2>cmdloopreturned0"
	echo "Captured stdout:"
        echo "Output: $output"
        echo "Exit Status: $status"
        echo "${stripped_output} -> ${expected_output}"


        [ "$stripped_output" = "$expected_output" ]
        [ "$status" -eq 0 ]

}

@test "arg too big" {
	run ./dsh <<EOF
cd aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
rc
EOF

	stripped_output=$(echo "$output" | tr -d '[:space:]')
        expected_output="dsh2>dsh2>-3dsh2>cmdloopreturned0"
        echo "Captured stdout:"
        echo "Output: $output"
        echo "Exit Status: $status"
        echo "${stripped_output} -> ${expected_output}"


        [ "$stripped_output" = "$expected_output" ]
        [ "$status" -eq 0 ]


}


@test "arg too big non built in command" {
        run ./dsh <<EOF
ls aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
rc
EOF

        stripped_output=$(echo "$output" | tr -d '[:space:]')
        expected_output="dsh2>dsh2>-3dsh2>cmdloopreturned0"
        echo "Captured stdout:"
        echo "Output: $output"
        echo "Exit Status: $status"
        echo "${stripped_output} -> ${expected_output}"


        [ "$stripped_output" = "$expected_output" ]
        [ "$status" -eq 0 ]


}

@test "cd with more than 2 arguments fails" {
	current=$(pwd)
	cd /tmp
	mkdir -p dir1
	mkdir -p dir1/dir2

	run "${current}/dsh" <<EOF
cd dir1 dir2
rc
pwd
EOF
        stripped_output=$(echo "$output" | tr -d '[:space:]')
	expected_output="/tmpdsh2>dsh2>-4dsh2>dsh2>cmdloopreturned0"
        echo "Captured stdout:"
        echo "Output: $output"
        echo "Exit Status: $status"
        echo "${stripped_output} -> ${expected_output}"


        [ "$stripped_output" = "$expected_output" ]
        [ "$status" -eq 0 ]

}

@test "cd to fake dir returns code -4" {
	current=$(pwd);
	run ./dsh <<EOF
cd fakedir
rc
pwd
EOF
        stripped_output=$(echo "$output" | tr -d '[:space:]')
        expected_output="${current}dsh2>dsh2>-4dsh2>dsh2>cmdloopreturned0"
        echo "Captured stdout:"
        echo "Output: $output"
        echo "Exit Status: $status"
        echo "${stripped_output} -> ${expected_output}"


        [ "$stripped_output" = "$expected_output" ]
        [ "$status" -eq 0 ]

}
