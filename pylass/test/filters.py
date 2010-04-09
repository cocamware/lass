import lass
import math
import random
import pylab
def lowpass_butterworth_quotient(n, w_c = 1, G = 1):
	if n % 2 == 1:
		denominator = lass.Polynomial([1, 1 / w_c])
	else:
		denominator = lass.Polynomial([1])
	for k in range(n / 2):
		denominator *= lass.Polynomial([1, -2 * math.cos(math.pi * (2 * k + n + 1) / (2 * n)) / w_c, 1 / (w_c * w_c)])	
	return lass.PolynomialQuotient(G, denominator)

def highpass_butterworth_quotient(n, w_c = 1, G = 1):
	a = lowpass_butterworth_quotient(n).denominator.coefficients
	a.reverse()
	for k in range(n + 1):
		a[k] /= (w_c ** k)
	b = n * [0] + [G / (w_c ** n)]
	return lass.PolynomialQuotient(b, a)

def makeRlcLowPass(w_c, Q, G, f_s):
		b = [G, 0,  G / (w_c ** 2)]
		a = [1, 1 / (Q * w_c), 1 / (w_c ** 2)]
		return lass.IirFilter.makeLaplace(b, a, f_s)
	
def makeAFilter(f_s):
		b = [0, 0, 0, 0, 7.39705e9]
		a = [.3086662405e21, .5301498365e19, .2674964995e17, .3343329206e14, 6734684542., 158881.5, 1]
		return lass.IirFilter.makeLaplace(b, a, f_s)


def fourier(ts, xs):
	def safe_dB(x):
		abs_x = abs(x)
		if abs_x < 1e-10:
			return -200
		return 20 * math.log10(abs_x)	
	n = len(ts)
	dt = ts[1] - ts[0]
	fs = [float(k+1)/(dt*n) for k in range(n/2)]
	Xs = [safe_dB(X) for X in pylab.fft(xs)[:n/2]]
	return fs, Xs



def rectangular_window(t, t_min, t_max):
	if t < t_min:
		return 0.
	if t < t_max:
		return 1.
	else:
		return 0
def hanning_window(t, t_min, t_max, t_fade):
	A, B = 0.5, 0.5
	if t < t_min:
		return 0.
	if t < t_min + t_fade:
		return 0.5 - 0.5 * math.cos(math.pi * (t - t_min) / t_fade)
	if t < t_max - t_fade:
		return 1.
	if t < t_max:
		return 0.5 - 0.5 * math.cos(math.pi * (t_max - t) / t_fade)
	else:
		return 0.	

def trapezium_window(t, t_min, t_max, t_fade):
	A, B = 0.5, 0.5
	if t < t_min:
		return 0.
	if t < t_min + t_fade:
		return (t - t_min) / t_fade
	if t < t_max - t_fade:
		return 1.
	if t < t_max:
		return (t_max - t) / t_fade
	else:
		return 0.
	
	
	
def a_weighting():
	print "a weighting ..."
	t_max = 1
	f_s = 44100
	n_s = t_max * f_s
	dt = 1.0 / f_s
	window = lambda t: hanning_window(t, 0.2 * t_max, 0.8 * t_max, 0.1 * t_max)
	
	ts = [float(i) * dt for i in xrange(n_s)]
	us = [window(t) * (2 * random.random() - 1) for t in ts]

	fs, Us = fourier(ts, us)

	fil = lass.IirFilter.makeAWeighting(f_s)
	ys = fil(us)
	fs, Ys = fourier(ts, ys)
	
	As = [Y - U for U, Y in zip(Us, Ys)]
	
	ax = pylab.axes()
	ax.grid(linestyle='-')
	pylab.semilogx(fs, As)
	pylab.xlim([10, 100e3])
	pylab.ylim([-50, 20])



def butterworth_transfer():
	print "butterworth transfer ..."
	n_s = 2 ** 14
	f_s = 10000
	max_order = 8
	f_c = 100
	G = 1
		
	dt = 1.0 / f_s
	w_c = 2 * math.pi * f_c

	t_max = n_s * dt
	window = lambda t: hanning_window(t, 0.2 * t_max, 0.8 * t_max, 0.1 * t_max)
	
	ts = [float(i) * dt for i in xrange(n_s)]
	us = [window(t) * (2 * random.random() - 1) for t in ts]
	
	fs, Us = fourier(ts, us)

	def run(maker, name):
		legends = []
		pylab.figure()
		pylab.title("transfer function %s" % name)
		for k in range(max_order):
			order = k + 1
			f = maker(order, w_c, G, f_s)
			ys = f(us)
			fs, Ys = fourier(ts, ys)
			As = [Y - U for U, Y in zip(Us, Ys)]
			pylab.semilogx(fs, As)
			legends.append('%sth order' % order)
		pylab.legend(legends)

	run(lass.IirFilter.makeButterworthLowPass, "low-pass butterworth")
	run(lass.IirFilter.makeButterworthHighPass, "high-pass butterworth")

		

def butterworth_step():
	print "butterworth step ..."
	n_s = 10000
	f_s = 10000
	max_order = 4
	f_c = 4
	G = 1

	dt = 1.0 / f_s
	w_c = 2 * math.pi * f_c
	
	t_max = n_s * dt
	window = lambda t: rectangular_window(t, 0.2 * t_max, 0.6 * t_max)

	ts = [float(i) * dt for i in xrange(n_s)]
	us = [window(t) for t in ts]
	
	def run(maker, name):
		pylab.figure()
		pylab.title("step response %s" % name)
		pylab.plot(ts, us)
		legends = ['input']
		for k in range(max_order):
			order = k + 1
			f = maker(order, w_c, G, f_s)
			ys = f(us)
			pylab.plot(ts, ys)
			legends.append('%sth order' % order)
		pylab.legend(legends)

	run(lass.IirFilter.makeButterworthLowPass, "low-pass butterworth")
	run(lass.IirFilter.makeButterworthHighPass, "high-pass butterworth")


def rlc_transfer():
	print "RLC transfer ..."
	n_s = 2 ** 14
	f_s = 10000
	max_order = 8
	f_c = 100
	G = 1
		
	dt = 1.0 / f_s
	w_c = 2 * math.pi * f_c

	t_max = n_s * dt
	window = lambda t: hanning_window(t, 0.2 * t_max, 0.8 * t_max, 0.1 * t_max)
	
	ts = [float(i) * dt for i in xrange(n_s)]
	us = [window(t) * (2 * random.random() - 1) for t in ts]
	
	fs, Us = fourier(ts, us)
	
	def run(maker, name):
		legends = []
		pylab.figure()
		pylab.title("transfer function %s" % name)
		for Q in [1 / math.sqrt(2), 1, 2, 5, 10, 20, 50]:
			f = maker(Q, w_c, G, f_s)
			ys = f(us)
			fs, Ys = fourier(ts, ys)
			As = [Y - U for U, Y in zip(Us, Ys)]
			pylab.semilogx(fs, As)
			legends.append('Q = %s' % Q)
		pylab.legend(legends)

	run(lass.IirFilter.makeRlcLowPass, "low-pass RLC")
	run(lass.IirFilter.makeRlcHighPass, "high-pass RLC")
	run(lass.IirFilter.makeRlcBandPass, "band-pass RLC")
	run(lass.IirFilter.makeRlcNotch, "notch RLC")


	
def integrator():
	print "integrator ..."
	f_s = 10000
	t_max = 3
	t_0 = -1
	window = lambda t: rectangular_window(t, 1, 2)
	
	dt = 1.0 / f_s
	n_s = t_max * f_s
	
	ts = [float(i) * dt for i in xrange(n_s)]
	us = [window(t) for t in ts]
	
	integrator = lass.IirFilter.makeIntegrator(1, f_s)
	ys = integrator(us)
	
	pylab.figure()
	pylab.title("integrator")
	pylab.plot(ts, us)
	pylab.plot(ts, ys)
	pylab.legend(["u", "y"])
	pylab.ylim(-0.2, 1.2)
	
	
def differentiator():
	print "differentiator ..."
	f_s = 10000
	t_max = 3
	window = lambda t: hanning_window(t, 0.8, 2.2, 0.4)
	
	dt = 1.0 / f_s
	n_s = t_max * f_s
	
	ts = [float(i) * dt for i in xrange(n_s)]
	us = [window(t) for t in ts]
	
	differentiator = lass.IirFilter.makeDifferentiator(1, f_s)
	ys = differentiator(us)
	
	pylab.figure()
	pylab.title("differentiator")
	pylab.plot(ts, us)
	pylab.plot(ts, ys)
	pylab.legend(["u", "y"])


	
	
if __name__ == "__main__":
	a_weighting()
	integrator()
	differentiator()	
	butterworth_step()
	butterworth_transfer()
	rlc_transfer()
	pylab.show()
