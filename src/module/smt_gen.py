import argparse
import sympy as sym

def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument('--file', default = "./benchmark/case05_cir1.poly")
    parser.add_argument('--ground', default = "./benchmark/case05_cir2.ground")
    parser.add_argument('--output', default = "./rewrite_smt.constraint")
    parser.add_argument('--output_word', default = "./rewrite_smt.word")
    parser.add_argument('--enable_signed', default=True)

    return parser.parse_args()

def declare(vars, type, output=None, silent=False):
    if not isinstance(vars, list):
        vars = [vars]
    for var in vars:
        if not silent:
            print("(declare-const {} {})".format(var, type))
        if output:
            output.write("(declare-const {} {})\n".format(var, type))

def assert_eq(expr, value, output=None, silent=False):
    if not silent:
        print("(assert (= {} {}))".format(value, expr))
    if output:
        output.write("(assert (= {} {}))\n".format(value, expr))

def assert_range(expr, lower, upper, output=None, silent=False):
    if not silent:
        print("(assert (and (>= {} {}) (< {} {})))".format(expr, lower, expr, upper))
    if output:
        output.write("(assert (and (>= {} {}) (< {} {})))\n".format(expr, lower, expr, upper))

def assert_imply(a, b, output=None, silent=False):
    if not silent:
        print("(assert (=> {} {}))".format(a, b))
    if output:
        output.write("(assert (=> {} {}))\n".format(a, b))

def createWord(name, inbit, outbit, enable_signed, output=None, silent=False):
    # Declare variables
    coeff_bit = [name + "_coeff_" + str(i) for i in range(inbit)]
    coeff = name + "_coeff"
    declare(coeff_bit, "Int", output, silent)
    declare(coeff, "Int", output, silent)

    neg = [name + "_neg_" + str(i) for i in range(inbit)]
    declare(neg, "Bool", output, silent)

    bit_coeff_bit = []
    for i in range(inbit):
        bit_coeff_bit.append([name + "_bit_" + str(i) + "_coeff_" + str(j) for j in range(outbit)])

    for bit_coeff in bit_coeff_bit:
        declare(bit_coeff, "Bool", output, silent)

    bit_coeff = [name + "_bit_" + str(i) for i in range(outbit)]
    declare(bit_coeff, "Bool", output, silent)

    signed = name + "_signed"
    declare(signed, "Bool", output, silent)

    # Generate Constraint
    for i in range(inbit):
        beta = name + "_beta_" + str(i)
        beta_expr = ""
        for j in range(outbit - enable_signed):
            if j == 0:
                beta_expr += bit_coeff_bit[i][j] + " "
            else:
                beta_expr += "(* {} {}) ".format(2**j, bit_coeff_bit[i][j])

        if (enable_signed):
            beta_expr += "(* (ite {} -{} {}) {})".format(signed, 2**(outbit-1), 2**(outbit-1), bit_coeff_bit[i][outbit-1])
        beta_expr = "(+ " + beta_expr + ")"
        declare(beta, "Int", output, silent)
        assert_eq(beta_expr, beta, output, silent)
        # if (enable_signed):
        #     assert_range(beta, -2**(outbit-1), 2**outbit, output)
        # else:
        #     assert_range(beta, 0, 2**outbit, output)

        constraint = "(ite {} (- {}) {})".format(neg[i], beta, beta)
        assert_eq(constraint, coeff_bit[i], output, silent)

    constraint = ""
    for i in range(inbit):
        constraint += "(* {} {}_beta_{}) ".format(neg[i], name, str(i))

    sub_expr = ""
    for j in range(outbit - enable_signed):
        if j == 0:
            sub_expr += bit_coeff[j] + " "
        else:
            sub_expr += "(* {} {}) ".format(2**j, bit_coeff[j])

    if (enable_signed):
        sub_expr += "(* (ite {} -{} {}) {})".format(signed, 2**(outbit-1), 2**(outbit-1), bit_coeff[outbit-1])

    constraint = "(+ " + constraint + sub_expr + ")"
    assert_eq(constraint, coeff, output, silent)

    for i in range(outbit):
        constraint = "(assert ((_ at-most 1) " + bit_coeff[i]
        for j in range(inbit):
            constraint += " " + bit_coeff_bit[j][i]
        constraint += "))"
        if not silent:
            print(constraint)
        if output:
            output.write(constraint + "\n")

    # Polynomial Generation
    coeff_bit_sym = [sym.Symbol(var) for var in coeff_bit]
    var_bit_sym = [sym.Symbol(name + "_" + str(i)) for i in range(inbit)]
    poly = sym.Symbol(name + "_coeff")
    for i in range(inbit):
        poly = poly + coeff_bit_sym[i] * var_bit_sym[i]

    return poly

def assert_coeff(expr, coeff, mod, output=None, silent=False):
    sub_expr = ""
    single = True
    for mono in expr.args:
        sub_sub_expr = ""
        for m in mono.args:
            sub_sub_expr += str(m) + " "
        if len(sub_sub_expr) == 0:
            single &= True
            sub_sub_expr = str(mono)
        else:
            single &= False
            sub_sub_expr = "(* " + sub_sub_expr + ")"
        sub_expr = sub_expr + " " + sub_sub_expr
    if single:
        sub_expr = "(*" + sub_expr + ")"
    else:
        sub_expr = "(+" + sub_expr + ")"

    if not silent:
        print("(assert (= {} (mod {} {})))".format(coeff, sub_expr, mod))
    if output:
        output.write("(assert (= {} (mod {} {})))\n".format(coeff, sub_expr, mod))

# a / b
def poly_div(a, b, ignore):
    monomials = a.args
    expr = sym.Integer(0)
    # print(a, b)
    if (a.is_Mul):
        if (a.has(b)):
            quo = a / b
            expr += quo.expand()
    else:
        for mono in monomials:
            # t = sym.div(mono, b)
            # print(t)
            if (mono.has(b)):
                # print("Has")
                quo = mono / b
                keep = True
                for q in quo.args:
                    if q in ignore:
                        keep = False
                        break
                if keep:
                    expr += quo.expand()
    
    return expr.expand()

def comment(describe, output = None, silent=False):
    if not isinstance(describe, str):
        describe = str(describe)
    if not silent:
        print("; {}".format(describe))
    if output:
        output.write("{}\n".format(describe))

def create_possibleList(bit_width1, bit_width2, polynomial1, polynomial2):
    possible_list = []
    for i in range(len(polynomial1)):
        temp_possible = []
        for j in range(len(polynomial2)):
            ## check whether term i can be mapped to term j
            # print(polynomial1[i] ,polynomial2[j])
            if(len(polynomial1[i][1]) == 0 and len(polynomial2[j][1]) == 0):
                if(polynomial1[i][0] != polynomial1[j][0]):
                    # print("const no match")
                    continue
            if(len(polynomial1[i][1]) != len(polynomial2[j][1])):## var num not equi
                # print("var num no match")
                continue
            monomial1 = []
            monomial2 = []
            for t in polynomial1[i][1]:
                monomial1.append(t[1])
            for t in polynomial2[j][1]:
                monomial2.append(t[1])
            monomial1.sort()
            monomial2.sort()
            if(monomial1 != monomial2):
                # print("var power no match")
                continue
            term_match = True
            for t in polynomial1[i][1]:
                power = t[1]
                bitwidth = bit_width1[t[0]]
                at_least_one = False
                for m in polynomial2[j][1]:
                    if((m[1] == power) and (bit_width2[m[0]] == bitwidth)):
                        at_least_one = True
                if(not at_least_one):
                    term_match = False
                    break
            if(term_match):
                temp_possible.append(j)
        possible_list.append(temp_possible)
    
    return possible_list

def rewrite_smt(args, silent=False):
    print("Generate SMT ... ", end='')
    with open(args.file, "r") as f:
        output = open(args.output, "w")
        output_word = open(args.output_word, "w")

        outbit = int(f.readline())
        var_num = int(f.readline())
        if output_word:
            comment(var_num, output_word, silent)

        # Read variables
        vars_vec = []
        vars = dict()
        for i in range(var_num):
            var = f.readline().rstrip().split(' ')
            vars[var[0]] = int(var[1])
            vars_vec.append(var[0])
            comment("{} {}".format(var[0], var[1]), output_word, silent)

        # Create Variables
        polys = dict()
        for var in vars.keys():
            bit = vars[var]
            polys[var] = createWord(var, bit, outbit, int(args.enable_signed), output, silent)

        # Read Support Information
        for i in range(var_num):
            uncertain = 0
            order = [[vars_vec[i] + "_" + str(j) for j in range(vars[vars_vec[i]])]]
            used = set()
            for j in range(vars[vars_vec[i]]):
                sup = f.readline().rstrip().split(" ")
                cur = sup.pop(0)
                sup.pop(0)
                # print(cur, sup)
                if len(sup) == 0:
                    uncertain += 1
                    continue
                index = -1
                for k in range(len(order)):
                    if cur in order[k]:
                        index = k
                        break
                popout = []
                used.add(cur)
                for k in range(len(sup)):
                    if sup[k].split("_")[0] == vars_vec[i]:
                        used.add(sup[k]) 
                    for l in range(0, index+1):
                        if sup[k] in order[l]:
                            order[l].remove(sup[k])
                            popout.append(sup[k])

                if index == len(order)-1:
                    order.append(popout)
                else:
                    order[index+1] += popout

            popout = []
            for j in range(len(order)):
                for k in range(len(order[j])):
                    if order[j][k] not in used:
                        popout.append(order[j][k])
                for pop in popout:
                    order[j].remove(pop)

            bucket_start = 0
            bucket_end = len(order[0])
            # print(order)
            for j in range(len(order)):
                # print(bucket_start, bucket_end, min(bucket_end + uncertain, vars[vars_vec[i]]))
                valid = range(j, min(bucket_end + uncertain, min(outbit, vars[vars_vec[i]])))
                for var in order[j]:
                    for l in range(min(outbit, vars[vars_vec[i]])):
                        if l in valid:
                            continue
                        if not silent:
                            print("(assert (not {}_bit_{}_coeff_{}))".format(vars_vec[i], var.split("_")[1], l))
                        if output:
                            output.write("(assert (not {}_bit_{}_coeff_{}))\n".format(vars_vec[i], var.split("_")[1], l))
                        
                bucket_start = bucket_end
                bucket_end += len(order[j+1]) if j < len(order)-1 else 0

            # print(order, uncertain)
            for j in range(len(order)):
                for k in range(0, j):
                    # bucket2 < bucket1
                    bucket1 = order[j]
                    bucket2 = order[k]
                    for b1 in bucket1:
                        var1 = vars_vec[i] + "_bit_" + b1.split("_")[1] + "_coeff_"
                        for b2 in bucket2:
                            var2 = vars_vec[i] + "_bit_" + b2.split("_")[1] + "_coeff_"
                            
                            for m in range(min(outbit, vars[vars_vec[i]])):
                                for n in range(m, min(outbit, vars[vars_vec[i]])):
                                    assert_imply(var1 + str(m), "(not " + var2 + str(n) + ")", output, silent)

        # Get Terms
        terms = f.readlines()
        terms = [term.rstrip() for term in terms]
        def countMul(term):
            return term.count("*")
        terms.sort(key=countMul, reverse=True)

        # Mono Variable
        term_set = set()
        for var in vars_vec:
            term_set.add(frozenset([var]))

        # Product Term
        # TODO: Power of variable
        for term in terms:
            t = term.split('*')
            t.pop(0)
            t = [x.split("_")[0] for x in t]
            term_set.add(frozenset(t))

        # Form Assumption
        # [(s_0, [(v_00, p_00), (v_01, p_01), ...]), (s_1, [(v_10, p_10), (v_11, p_11), ...]), ...]
        assume_form = []
        mono_num = 0
        poly_assume = sym.Integer(0)
        for term in term_set:
            s = "s_" + str(mono_num)
            # constraint = "(assert (and (>= {} 0) (< {} {})))".format(s, s, 2**outbit)
            declare(s, "Int", output, silent)
            mono_num += 1
            monos = []
            expr = "; " + s
            expr_sym = sym.Symbol(s)
            for t in term:
                monos.append((t, 1))
                expr += " * " + t
                expr_sym *= polys[t]
            if not silent:
                print(expr)
            if output:
                output.write(expr + "\n")
            assume_form.append((s, monos))
            expr_sym = expr_sym.expand()
            poly_assume += expr_sym

        poly_assume = poly_assume.as_expr()

        # Coefficient Calculation
        all_vars = []
        for var in vars.keys():
            bit = vars[var]
            for b in range(bit):
                all_vars.append(sym.Symbol(var + "_" + str(b)))

        for term in terms:
            t = term.split('*')

            if len(t) > 1:
                coeff = int(t.pop(0)) % (2**outbit)
                coeff = coeff if coeff >= 0 else coeff + 2**outbit
                expr = sym.Integer(1)
                for m in t:
                    expr *= sym.Symbol(m)
                expr = expr.as_expr()

                quo = poly_div(poly_assume, expr, all_vars)
                
                assert_coeff(quo, coeff, 2**outbit, output, silent)

                poly_assume = poly_assume - (quo * expr)
                poly_assume = poly_assume.expand()
            else:
                coeff = int(t.pop(0)) % (2**outbit)
                coeff = coeff if coeff >= 0 else coeff + 2**outbit
                const = sym.Integer(0)
                monomials = poly_assume.args
                for mono in monomials:
                    keep = True
                    for m in mono.args:
                        if m in all_vars:
                            keep = False
                            break
                    if keep:
                        const = const + mono

                assert_coeff(const, coeff, 2**outbit, output, silent)
                poly_assume = poly_assume - const
                poly_assume = poly_assume.expand()

        # Assert unused coefficient to be 0
        all_combs = []
        for term in term_set:
            term = list(term)
            if len(term) == 0:
                continue
            if len(term) == 1:
                var = term[0]
                for i in range(vars[var]):
                    all_combs.append(sym.Symbol(var + "_" + str(i)))
            else:
                partial_comb = []
                var1 = term[0]
                var2 = term[1]
                for i in range(vars[var1]):
                    var1_sym = sym.Symbol(var1 + "_" + str(i))
                    for j in range(vars[var2]):
                        var2_sym = sym.Symbol(var2 + "_" + str(j))
                        partial_comb.append(var1_sym * var2_sym)

                for i in range(2, len(term)):
                    partial_comb_updated = []
                    var = term[i]
                    for j in range(vars[var]):
                        var_sym = sym.Symbol(var + "_" + str(j))
                    for p in partial_comb:
                        updated = p * var_sym
                        partial_comb_updated.append(updated.expand())
                    partial_comb = partial_comb_updated

                all_combs += partial_comb

        for comb in all_combs:
            quo = poly_div(poly_assume, comb, all_vars)
            # print(comb, quo)
            if quo == 0:
                continue

            assert_coeff(quo, 0, 2**outbit, output, silent)
            poly_assume = poly_assume - (quo * comb)
            poly_assume = poly_assume.expand()

        if poly_assume != 0:
            assert_coeff(poly_assume, 0, 2**outbit, output, silent)

        # print(all_combs)

        # if not silent:
        #     print("(check-sat)\n(get-model)")
        # if output:
        #     output.write("(check-sat)\n(get-model)\n")

    print("done")

    return vars, assume_form

def read_cir2(filepath):
    with open(filepath, "r") as f:
        outbit = int(f.readline())
        var_num = int(f.readline())

        # Read variables
        vars_vec = []
        vars = dict()
        for i in range(var_num):
            var = f.readline().rstrip().split(' ')
            vars[var[0]] = int(var[1])
            vars_vec.append(var[0])

        terms = f.readlines()
        terms = [term.rstrip() for term in terms]
        def countMul(term):
            return term.count("*")
        terms.sort(key=countMul, reverse=True)

        form = []

        for term in terms:
            t = term.split('*')
            coeff = int(t.pop(0))
            t = [x.split("_")[0] for x in t]
            t = [(x, 1) for x in t]
            form.append((coeff, t))

    return vars, form

def one_step_constraint(assume_form, assume_vars, reference_form, reference_vars, possible_list, args, silent=False):
    with open(args.output, "a") as f:
        print(f)
        reference_vars = [key for key in reference_vars.keys()]
        assume_vars = [key for key in assume_vars.keys()]

        vars = []
        for ref in assume_vars:
            for asm in reference_vars:
                declare("vars_{}_{}".format(ref, asm), "Bool", f, silent)
            vars.append(["vars_{}_{}".format(ref, asm) for asm in reference_vars])

        terms = []
        for i in range(len(assume_form)):
            for j in range(len(reference_form)):
                declare("term_{}_{}".format(i, j), "Bool", f, silent)
            terms.append(["term_{}_{}".format(i, j) for j in range(len(reference_form))])

        for var in assume_vars:
            declare("neg_{}".format(var), "Bool", f, silent)

        # print(assume_form)
        for i in range(len(possible_list)):
            if len(possible_list[i]) == 0:
                assert_eq(assume_form[i][0], 0, f, silent)
                continue
            for j in possible_list[i]:
                # term negation implication
                trans_var = []
                for term in assume_form[i][1]:
                    if term[1] % 2:
                        trans_var.append(term[0])

                neg_cond = ""
                for var in trans_var:
                    neg_cond += " neg_" + var
                neg_cond = "(xor" + neg_cond + ")"
                neg_imp = "(assert (=> {} (= {} (ite {} -{} {}))))".format(terms[i][j], assume_form[i][0], neg_cond, reference_form[j][0], reference_form[j][0])
                if not silent:
                    print(neg_imp)
                if f:
                    f.write("{}\n".format(neg_imp))
                var_cond = ""
                for m in range(len(assume_form[i][1])):
                    at_least_cond = ""
                    for n in range(len(reference_form[j][1])):
                        at_least_cond += " vars_" + assume_form[i][1][m][0] + "_" + reference_form[j][1][n][0]
                        # print(assume_form[i][1][m][0], reference_form[j][1][n][0])
                    at_least_cond = "((_ at-least 1)" + at_least_cond + ")"
                    var_cond += " " + at_least_cond
                
                var_cond = "(and" + var_cond + ")"
                if not silent:
                    print("(assert (=> {} {}))".format(terms[i][j], var_cond))
                if f:
                    f.write("(assert (=> {} {}))\n".format(terms[i][j], var_cond))
                # print(var_cond)

        vars_cond = ""
        for i in range(len(reference_vars)):
            var_cond = ""
            for j in range(len(assume_vars)):
                var_cond += " " + vars[j][i]
            var_cond = "((_ at-most 1)" + var_cond + ")"
            vars_cond += " " + var_cond

        vars_cond = "(and" + vars_cond + ")"
        if not silent:
            print("(assert {})".format(vars_cond))
        if f:
            f.write("(assert {})\n".format(vars_cond))

        term_cond = ""
        for i in range(len(possible_list)):
            if len(possible_list[i]) == 0:
                continue
            at_least_cond = ""
            for j in possible_list[i]:
                at_least_cond += " " + terms[i][j]
            at_least_cond = "((_ at-least 1)" + at_least_cond + ")"
            term_cond += " " + at_least_cond

        for i in range(len(reference_form)):
            at_most_cond = ""
            for (index_assume, l) in enumerate(possible_list):
                if len(l) == 0:
                    continue
                if i in l:
                    # print(terms[index_assume][i])
                    at_most_cond += " " + terms[index_assume][i]
            at_most_cond = "((_ at-most 1)" + at_most_cond + ")"
            term_cond += " " + at_most_cond

        term_cond = "(and" + term_cond + ")"
        if not silent:
            print("(assert {})".format(term_cond))
        if f:
            f.write("(assert {})\n".format(term_cond))


def main():
    args = parse_arguments()

    bit_widths_1, assume_form = rewrite_smt(args=args, silent=False)
    bit_widths_2, reference_form = read_cir2(filepath=args.ground)

    possible_list = create_possibleList(bit_width1=bit_widths_1, bit_width2=bit_widths_2, polynomial1=assume_form, polynomial2=reference_form)

    one_step_constraint(assume_form, bit_widths_1, reference_form, bit_widths_2, possible_list, args)

    with open(args.output, "a") as output:
        output.write("(check-sat)\n")
        output.write("(get-model)\n")

    

if __name__ == '__main__':
    main()