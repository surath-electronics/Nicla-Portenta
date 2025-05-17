import json

def analyze_sequence_gaps(json_file_path):
    # Load the JSON file
    with open(json_file_path, 'r') as file:
        data = json.load(file)

    # Extract all sequence numbers in the order they appear
    seq_list = [entry['seq'] for entry in data if 'seq' in entry]

    if not seq_list:
        print("No 'seq' values found.")
        return

    dropped_count = 0
    missing_sequences = []

    for i in range(1, len(seq_list)):
        expected = seq_list[i-1] + 1
        actual = seq_list[i]
        if actual != expected:
            gap = actual - expected
            dropped_count += gap
            missing_sequences.extend(range(expected, actual))

    total_expected = (seq_list[-1] - seq_list[0]) + 1
    actual_count = len(seq_list)
    drop_percentage = (dropped_count / total_expected) * 100

    # Print results
    print(f"First Sequence: {seq_list[0]}")
    print(f"Last Sequence: {seq_list[-1]}")
    print(f"Expected Total Sequences: {total_expected}")
    print(f"Actual Sequences Present: {actual_count}")
    print(f"Total Dropped Sequences: {dropped_count}")
    print(f"Dropped Percentage: {drop_percentage:.2f}%")

    # Optionally print missing sequences
    # print(f"Missing Sequences: {missing_sequences}")

# Example usage
analyze_sequence_gaps("test.portenta_stream.json")
