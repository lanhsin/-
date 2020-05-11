#! usr/bin/python

import getopt, sys, os, re

#type_uint32 =     'unsigned       int'
#type_uint8  =     'unsigned      char'
type_size =       '      unsigned    int'
type_pointer =    'const unsigned char *'
#type_pointer =    '      unsigned    int'
type_data_array = 'const unsigned   char'

mandatory_block_names = ['Descriptor', 'Data In', 'Data Out']
decimal_block_names = ['IsMasterMode', 'IsLastPacket']

def parse_options (argv):
    help_message = 'Usage: python %s -t <test_name> [-s <split_test_case_number>] [-z <split_test_pattern_size>] <input_file>' % (os.path.basename(__file__))

    if len(argv) < 3:
        print help_message
        sys.exit()

    input_file = argv.pop()

    test_name = ''
    result_file = ''
    test_case_number = None
    test_pattern_size = None
    
    try:
        opts, args = getopt.getopt(argv,"ht:s:z:",["help", "split"])
    except getopt.GetoptError:
        print help_message
        sys.exit(2)
        
    for opt, arg in opts:
        if opt in ('-h', '--help'):
            print help_message
            sys.exit()
        elif opt in ('-t'):
            test_name = arg
        elif opt in ('-z'):
            test_pattern_size = int(arg)
        elif opt in ('-s', 'split'):
            test_case_number = int(arg)
            
    return input_file, test_name, test_case_number, test_pattern_size


def output (file, line):
    #print line
    file.write('%s\n' % line)

def get_array_name (block_name, test_name, test_case_number=None):
    try:
        return re.sub('[\s\.]', '_', ' '.join([test_name, block_name, '%04d' % (test_case_number + 1)]))
    except:
        return re.sub('[\s\.]', '_', ' '.join([block_name]))

def get_macro_array_name (test_name):
    return re.sub('[\s\.]', '_', ' '.join([test_name, 'macro']))

def get_macro_structure_name (test_name):
    return re.sub('[\s\.]', '_', ' '.join(['struct', 'sec', 'test case', 'macro']))

def output_block (file_out, data, test_name, block_name, test_case_number):
    
    if (block_name not in data.keys()) or (test_case_number not in data[block_name].keys()):
        return
    
    output(file_out, '%s %s[] =' % (type_data_array, get_array_name(block_name, test_name, test_case_number)))
    output(file_out, '{')
    line_head = '  '
    for l in data[block_name][test_case_number]:
        output(file_out, '  %s%s' % (line_head, ', '.join(map(lambda x: '0x'+x, l))))
        line_head = ', '
    output(file_out, '};')
    output(file_out, '')
    return

def is_cluster_start (data, index, mode, master, last):
    
    if data['IsMasterMode'].has_key(index):
        if data['IsMasterMode'][index][0][0] == '00':
            master[0] = False
        else:
            master[0] = True
    
    if data['IsLastPacket'].has_key(index):
        if data['IsLastPacket'][index][0][0] == '00':
            last[0] = False
        else:
            last[0] = True
    
    if master[0] == False:
        mode[0] = 'slave'
        return True
    else:
        if mode[0] != 'master':
            mode[0] = 'master'
            return True
        else:
            return False
    

def is_cluster_end (data, index, mode, master, last):
    #print mode, master, last

    if mode[0] == 'slave':
        #mode[0] = 'slave end'
        return True
    
    else:
        if last[0] == True:
            mode[0] = 'slave'
            return True
        else:
            return False
    

def get_block_size (block):
    return reduce(lambda x,y:x+y, map(lambda x: len(x), block))

def calculate_test_pattern_size (data, index, block_names):
    size = 0
    for block_name in block_names:
        if data[block_name].has_key(index):
            size = size + get_block_size(data[block_name][index])
    return size

def calculate_cluster_size (data, ran, block_names):
    cluster_size = {}
    cluster_start_index = ran[0]
    mode = ['']
    master = ['']
    last = ['']
    for index in ran:
        if is_cluster_start(data, index, mode, master, last) == True:
            cluster_start_index = index
            cluster_size[cluster_start_index] = 0

        cluster_size[cluster_start_index] = cluster_size[cluster_start_index] + calculate_test_pattern_size(data, index, block_names)
        
        if is_cluster_end(data, index, mode, master, last) == True:
            pass

    return cluster_size

def get_last_block (data, block_name, ran):
    return data[block_name][filter(lambda x:x in ran, sorted(data[block_name].keys()))[-1]]

def process_input_file (input_file):
    # Process input file to generate test pattern
    try:
        file_in = open(input_file, 'r')
    except IOError as error_message:
        print error_message
        sys.exit()

    data = {}
    line_number = 0
    block_name = ''
    test_case_number = 0
    block_names = []

    for line in file_in:
    
        line_number = line_number + 1
        
        # Removes all whitespace characters on the right side of a string
        line = line.rstrip()
        
        # Comment of a block. Treat it as the block name
        m = re.match('^//\s*([a-zA-Z_\s]+)(\s+(\d+))?$', line)
        if m != None:
            if (test_case_number != 0) and (block_name == block_names[-1]):
                test_case_number = test_case_number + 1
                
            block_name = m.group(1)
            
            if block_name not in block_names:
                if test_case_number != 0:
                    print 'Check block name %s in #%d' % (block_name, line_number)
                    sys.exit()
                block_names.append(block_name)
            elif test_case_number == 0:
                test_case_number = test_case_number + 1
            
            if (m.group(2) != None) and ((test_case_number + 1) != int(m.group(2))):
                print 'Test case number is mismatched: current test number is %d, and number in block name is %d' % (test_case_number, int(m.group(2)))
                sys.exit()

            # Initialize data[block_name][test_case_number] for storing block content
            if block_name not in data.keys():
                data[block_name] = {}
            data[block_name][test_case_number] = []
                
            continue 

        m = re.match('^[0-9a-fA-F]', line)
        if m != None:
            # // INT_ST 1
            # 2147483648
            if block_name in decimal_block_names:
                data[block_name][test_case_number].append(map(lambda x: '%02x' % (int(x)), re.split('\s+', line)))

            else:                
                data[block_name][test_case_number].append(re.split('\s+', line))
            
            continue

    if block_name != block_names[-1]:
        print 'The last block (\'%s\') in input file is not the last block of a test case' % (block_name)
        sys.exit()
        
    file_in.close()

    return (data, test_case_number + 1, block_names)

def remove_duplication (data, total_test_case_number, block_names):
    for block_name in block_names:
        
        if block_name in mandatory_block_names:
            continue
        
        for test_case_number in range(total_test_case_number - 1, 0, -1):
            if data[block_name].has_key(test_case_number) == True:
                if data[block_name][test_case_number] == get_last_block(data, block_name, range(0, test_case_number)):
                    data[block_name].pop(test_case_number)

def derive_test_case_start_indexes (data, total_test_case_number, block_names, test_pattern_size_in_one_file, test_case_number_in_one_file):
    if (test_pattern_size_in_one_file != None) and (test_pattern_size_in_one_file > 1024):
        cluster_size = calculate_cluster_size(data, range(0, total_test_case_number), block_names)
        test_case_start_indexes = [0]
        test_pattern_size = 0
        for i in sorted(cluster_size.keys()):
            if (test_pattern_size + cluster_size[i]) > test_pattern_size_in_one_file:
                if i <= test_case_start_indexes[-1]:
                    print 'test_pattern_size_in_one_file %d is smaller than one cluster %d: test case %d' % (test_pattern_size_in_one_file, cluster_size[i], i)
                    sys.exit()
                test_case_start_indexes.append(i)
                test_pattern_size = 0
            else:
                test_pattern_size = test_pattern_size + cluster_size[i]
        
    elif (test_case_number_in_one_file != None) and (test_case_number_in_one_file != 0):
        test_case_start_indexes = range(0, total_test_case_number, test_case_number_in_one_file)
    else:
        test_case_start_indexes = [0]
        
    return test_case_start_indexes

def generate_header_file (data, block_names, test_case_range, test_name, file_index):
    test_case_start = test_case_range[0]
    
    for block_name in block_names:
        if data[block_name].has_key(test_case_start) != True:
            data[block_name][test_case_start] = get_last_block(data, block_name, range(0, test_case_start))
    
    #test_case_range = range(test_case_start, min(test_case_start + test_case_number_in_one_file, total_test_case_number))
    for test_case_number in test_case_range:
        #if test_case_number % test_case_number_in_one_file == 0:
        if test_case_number == test_case_start:
            file_name = test_name + '_%03d.h' % (file_index)
            file_out = open(file_name, 'w')
            output(file_out, '#ifndef __%s_%03d__' % (test_name, file_index))
            output(file_out, '#define __%s_%03d__' % (test_name, file_index))
            output(file_out, '')
    
        for block_name in block_names:
            output_block(file_out, data, test_name, block_name, test_case_number)

    output(file_out, 'const static %s %s[] =' % (get_macro_structure_name(test_name), get_macro_array_name(test_name) + '_%03d' % (file_index)))
    output(file_out, '{')
    line_head = '  '
    for test_case_number in test_case_range:
        output(file_out, '  %s{' % (line_head))
        for block_name in block_names:
            if (block_name not in data.keys()) or (test_case_number not in data[block_name].keys()):
                output(file_out, '        %5d, (%s)NULL,' % (0, type_pointer))
            else:
                output(file_out, '        %5d, (%s)(&(%s[0])),' % (get_block_size(data[block_name][test_case_number]), type_pointer, get_array_name(block_name, test_name, test_case_number)))
                
        output(file_out, '    }')
        line_head = ', '
    output(file_out, '};')
    output(file_out, '')
    
    output(file_out, '#endif')
    output(file_out, '')
    
    file_out.close()


def transform (options):
    (input_file, test_name, test_case_number_in_one_file, test_pattern_size_in_one_file) = options

    # Process input file to generate test pattern
    data, total_test_case_number, block_names = process_input_file(input_file)
    
    # Remove duplication
    remove_duplication(data, total_test_case_number, block_names)
    
    test_case_start_indexes = derive_test_case_start_indexes(data, total_test_case_number, block_names, test_pattern_size_in_one_file, test_case_number_in_one_file)

    print "Test case start index:", 
    print test_case_start_indexes

    # To guarantee all blocks exist in the first test case in each file
    
    file_index = -1
    # for each output file
    for i in range(0, len(test_case_start_indexes)):
        file_index = file_index + 1
        try:
            test_case_range = range(test_case_start_indexes[i], test_case_start_indexes[i + 1])
        except:
            test_case_range = range(test_case_start_indexes[i], total_test_case_number)

        generate_header_file(data, block_names, test_case_range, test_name, file_index)

    print ''
    print '============================================'
    print 'typedef struct'
    print '{'
    for block_name in block_names:
        print '    %s %s_size;' % (type_size, get_array_name(block_name, test_name))
        print '    %s %s_ptr;' % (type_pointer, get_array_name(block_name, test_name))
    print '} %s;' % (get_macro_structure_name(test_name))
    print '============================================'

    return



if __name__ == "__main__":
    #input_file, output_file, result_file = parse_options(sys.argv[1:])
    options = parse_options(sys.argv[1:])
    print ''
    print 'Input file:              %s\nTest name:               %s' % (options[0], options[1])
    print 'Split test case number: ',
    print options[2]
    print 'Split test pattern size:',
    print options[3]
    print ''
    transform(options)
    print ''
    print 'Done'
    print ''
    